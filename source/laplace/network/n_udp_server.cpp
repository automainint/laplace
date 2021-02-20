/*  laplace/network/n_udp_server.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../core/utils.h"
#include "../engine/prime_impact.h"
#include "../engine/protocol/ids.h"
#include "../engine/protocol/ping.h"
#include "../engine/protocol/request_events.h"
#include "../engine/protocol/server_idle.h"
#include "../engine/protocol/slot_create.h"
#include "udp_server.h"
#include <algorithm>

namespace laplace::network {
  namespace access   = engine::access;
  namespace protocol = engine::protocol;
  namespace ids      = protocol::ids;

  using std::min, std::any_of, std::find_if, std::span,
      std::numeric_limits, std::string, std::string_view,
      engine::prime_impact, protocol::slot_create,
      protocol::request_events, protocol::server_idle,
      protocol::ping_request, engine::time_undefined,
      engine::id_undefined, engine::encode;

  void udp_server::set_allowed_commands(cref_vuint16 commands) {
    m_allowed_commands.assign(commands.begin(), commands.end());
  }

  void udp_server::set_chunk_size(size_t size, size_t overhead) {
    m_overhead = overhead;
    m_buffer.resize(size + overhead);
  }

  void udp_server::tick(size_t delta_msec) {
    receive_events();

    process_slots();

    update_slots(delta_msec);

    send_events();

    update_world(delta_msec);

    update_local_time(delta_msec);
  }

  auto udp_server::get_port() const -> uint16_t {
    return m_node ? m_node->get_port() : any_port;
  }

  auto udp_server::perform_control(size_t slot, cref_vbyte seq)
      -> bool {
    return false;
  }

  auto udp_server::get_local_time() const -> uint64_t {
    return m_local_time;
  }

  auto udp_server::get_chunk_size() const -> size_t {
    return m_buffer.size();
  }

  auto udp_server::adjust_chunk_size(cref_vbyte chunk) const -> size_t {
    size_t chunk_size = 0;

    while (chunk_size + 2 < chunk.size()) {
      size_t size = rd<uint16_t>(chunk, chunk_size);

      if (chunk_size + size + 2 + m_overhead > m_buffer.size())
        break;

      chunk_size += size + 2;
    }

    return chunk_size;
  }

  void udp_server::update_world(size_t delta_msec) {
    if (m_is_distribution_enabled) {
      if (get_state() == server_state::action) {
        if (auto sol = get_solver(); sol) {
          sol->schedule(adjust_delta(delta_msec));
        } else {
          error(__FUNCTION__, "No solver.");
          set_state(server_state::pause);
        }
      }
    } else {
      if (get_state() == server_state::action) {
        if (auto sol = get_solver(); sol) {
          sol->schedule(convert_delta(delta_msec));
        } else {
          error(__FUNCTION__, "No solver.");
          set_state(server_state::pause);
        }
      }
    }
  }

  void udp_server::send_events() {
    while (m_queue.index < m_queue.events.size()) {
      send_event(m_queue.events[m_queue.index++]);
    }

    send_chunks();
  }

  void udp_server::send_event_to(size_t slot, cref_vbyte seq) {
    if (slot_create::scan(seq)) {
      auto buf = vbyte(seq.begin(), seq.end());
      slot_create::alter(buf, m_slots[slot].id_actor);

      append_event(slot, buf);
    } else {
      append_event(slot, seq);
    }
  }

  void udp_server::send_event(cref_vbyte seq) {
    if (slot_create::scan(seq)) {
      auto buf = vbyte(seq.begin(), seq.end());

      for (size_t i = 0; i < m_slots.size(); i++) {
        slot_create::alter(buf, m_slots[i].id_actor);
        append_event(i, buf);
      }
    } else {
      for (size_t i = 0; i < m_slots.size(); i++) {
        append_event(i, seq);
      }
    }
  }

  void udp_server::append_event(size_t slot, cref_vbyte seq) {
    if (m_slots[slot].is_connected) {
      auto &chunk = m_slots[slot].chunks;

      const auto size   = seq.size();
      const auto offset = chunk.size();

      if (size <= numeric_limits<uint16_t>::max()) {
        chunk.resize(offset + n_command + size);
        write_bytes(
            span<uint8_t> { chunk.begin() + offset, chunk.end() },
            static_cast<uint16_t>(size), seq);
      } else {
        error(__FUNCTION__, "Incorrect event size: %zu.", size);
      }
    }
  }

  void udp_server::set_max_slot_count(size_t count) {
    m_max_slot_count = count;
  }

  void udp_server::set_distribution_enabled(bool is_enabled) {
    m_is_distribution_enabled = is_enabled;
  }

  void udp_server::set_performing_enabled(bool is_enabled) {
    m_is_performing_enabled = is_enabled;
  }

  auto udp_server::is_allowed(size_t command_id) const -> bool {
    return m_allowed_commands.empty() ||
           any_of(m_allowed_commands.begin(),
                  m_allowed_commands.end(), [command_id](auto id) {
                    return id == command_id;
                  });
  }

  auto udp_server::is_join_enabled() const -> bool {
    return m_max_slot_count > 0;
  }

  auto udp_server::add_slot(std::string_view address, uint16_t port)
      -> size_t {

    const auto id = m_slots.size();
    m_slots.emplace_back(
        slot_info { .address = string(address), .port = port });
    return id;
  }

  auto udp_server::find_slot(string_view address, uint16_t port)
      -> size_t {

    for (size_t i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port) {
        m_slots[i].is_connected = true;
        m_slots[i].wait         = 0;

        if (!is_join_enabled())
          set_connected(true);

        return i;
      }
    }

    if (!is_join_enabled()) {
      error(__FUNCTION__, "Joining is disabled.");
      return -1;
    }

    if (!has_free_slots()) {
      error(__FUNCTION__, "No free slots.");
      return -1;
    }

    return add_slot(address, port);
  }

  void udp_server::process_slots() {
    for (size_t i = 0; i < m_slots.size(); i++) {
      process_queue(i);
      check_outdate(i);
    }

    clean_slots();
  }

  void udp_server::process_queue(size_t slot) {
    auto & qu = m_slots[slot].queue;
    size_t n  = 0;

    for (; n < qu.events.size(); n++) {
      auto &seq = qu.events[n];
      if (seq.empty())
        break;
      process_event(slot, seq);
    }

    if (n < qu.events.size() && !m_slots[slot].request_flag) {
      vuint events;

      for (size_t i = n; i < qu.events.size(); i++) {
        if (qu.events[i].empty()) {
          events.emplace_back(qu.index + i);
        }

        if (events.size() >= request_events::max_event_count) {
          break;
        }
      }

      send_event_to( //
          slot,      //
          encode<request_events>(events));

      m_slots[slot].request_flag = true;
    }

    qu.events.erase(qu.events.begin(), qu.events.begin() + n);

    qu.index += n;
  }

  void udp_server::check_outdate(size_t slot) {
    if (m_is_distribution_enabled &&
        m_slots[slot].outdate >= get_update_timeout()) {

      if (auto sol = get_solver(); sol) {
        send_event_to( //
            slot,      //
            encode<server_idle>(sol->get_time()));
      }

      m_slots[slot].outdate = 0;
    }
  }

  void udp_server::clean_slots() {
    if (is_join_enabled()) {
      m_slots.erase(           //
          std::remove_if(      //
              m_slots.begin(), //
              m_slots.end(),   //
              [](const auto &s) {
                return s.id_actor == id_undefined;
              }),
          m_slots.end());
    }
  }

  void udp_server::process_event(size_t slot, cref_vbyte seq) {
    if (prime_impact::get_time(seq) == time_undefined) {
      if (perform_control(slot, seq))
        return;

      if (m_is_distribution_enabled) {
        distribute_event(slot, seq);
      } else {
        instant_event(slot, seq);
      }

    } else {
      if (m_is_performing_enabled) {
        perform_event(slot, seq);
      } else {
        verb("[ server ] ignore timed event");
      }
    }
  }

  void udp_server::distribute_event(size_t slot, cref_vbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("[ server ] unable to decode command");
        return;
      }

      /*  Set the event actor id.
       */
      if (slot != slot_host)
        ev->set_actor(m_slots[slot].id_actor);

      /*  Set the event index.
       */
      ev->set_order({ m_queue.events.size() });

      /*  Add the event to the main queue.
       */
      m_queue.events.emplace_back(ev->encode());

      if (get_state() == server_state::prepare) {
        if (auto wor = get_world(); wor) {
          /*  Perform the event. The event
           *  time will be undefined.
           */
          ev->perform({ *wor, access::sync });
        }
      } else {
        if (auto sol = get_solver(); sol) {
          /*  Apply the event and
           *  set the event time.
           */
          sol->apply(ev);
        }
      }
    } else {
      error(__FUNCTION__, "No factory.");
    }
  }

  void udp_server::instant_event(size_t slot, cref_vbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("[ server ] unable to decode command");
        return;
      }

      ev->perform({ *get_world(), access::sync });

    } else {
      error(__FUNCTION__, "No factory.");
    }
  }

  void udp_server::perform_event(size_t slot, cref_vbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("[ server ] unable to decode command");
        return;
      }

      update_time_limit(ev->get_time());

      if (auto sol = get_solver(); sol) {
        sol->apply(ev);
      }
    }
  }

  void udp_server::receive_chunks() {
    if (m_node) {
      for (;;) {
        auto n = m_node->receive_to(
            m_buffer.data(), get_chunk_size(), async);
        if (n == 0)
          break;

        auto slot = find_slot(            //
            m_node->get_remote_address(), //
            m_node->get_remote_port());

        if (slot >= m_slots.size()) {
          error(__FUNCTION__, "Unable to find slot.");
          continue;
        }

        if (m_slots[slot].cipher.is_ready()) {
          dump({ m_buffer.data(), n });
        }

        auto plain = m_slots[slot].cipher.decrypt(
            { m_buffer.data(), n });

        if (plain.empty()) {
          verb("[ server ] unable to decrypt chunk");
          continue;
        }

        dump(plain);

        if (is_verbose()) {
          verb("[ server ] RECV %zu bytes on slot %zu", plain.size(),
               slot);
        }

        m_slots[slot].buffer.insert(    //
            m_slots[slot].buffer.end(), //
            plain.begin(),              //
            plain.end());

        m_slots[slot].request_flag = false;
      }
    }
  }

  void udp_server::add_event(size_t slot, cref_vbyte seq) {
    auto  index = prime_impact::get_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == -1) {
      /*  For public_key, request_events and client_ping.
       */
      if (!perform_control(slot, seq)) {
        verb("[ server ] ignore unindexed command on slot %zu.", slot);
      }

    } else if (index >= qu.index) {
      auto n = index - qu.index;

      if (n >= qu.events.size()) {
        qu.events.resize(n + 1);
      }

      if (qu.events[n].empty()) {
        qu.events[n].assign(seq.begin(), seq.end());
        m_slots[slot].outdate = 0;
      } else {
        verb("[ server ] event %zu already written on slot %zu.",
             index, slot);
      }

    } else {
      verb("[ server ] event %zu duplicate on slot %zu", index, slot);
    }
  }

  void udp_server::receive_events() {
    receive_chunks();
    process_buffers();
  }

  void udp_server::process_buffers() {
    for (size_t slot = 0; slot < m_slots.size(); slot++) {
      auto & buf   = m_slots[slot].buffer;
      size_t index = 0;

      while (buf.size() - index > n_command) {
        const size_t size = rd<uint16_t>(buf, index);

        if (buf.size() - index < n_command + size) {
          verb("[ server ] ignore corrupted data");

          buf.clear();
          break;
        }

        const auto id = prime_impact::get_id(
            { buf.begin() + index + n_command, buf.end() });

        if (is_allowed(id)) {
          add_event(slot, { buf.data() + index + n_command, size });
        } else {
          if (is_verbose()) {
            if (id < ids::_native_count) {
              verb("[ server ] command '%s (%hu)' not allowed",
                   ids::table[id].data(), id);
            } else {
              verb("[ server ] command '%hu' not allowed", id);
            }
          }
        }

        index += n_command + size;
      }

      if (buf.size() >= index) {
        buf.erase(buf.begin(), buf.begin() + index);
      }
    }
  }

  void udp_server::send_chunks() {
    for (auto &s : m_slots) {
      auto &buf = s.chunks;

      if (buf.empty())
        continue;

      auto chunk_size = adjust_chunk_size(buf);

      if (chunk_size == 0) {
        verb("[ server ] unable to adjust chunk size");

        buf.clear();
        continue;
      }

      dump({ buf.data(), chunk_size });

      if (s.is_encrypted) {
        auto chunk = s.cipher.encrypt({ buf.data(), chunk_size });

        if (chunk.size() == 0) {
          verb("[ server ] unable to encrypt chunk");

          buf.clear();
          continue;
        }

        dump(chunk);

        if (is_verbose()) {
          verb("[ server ] SEND %zu bytes to %s:%hu (encrypted)",
               chunk.size(), s.address.c_str(), s.port);
        }

        m_node->send_to(s.address, s.port, chunk);
      } else {
        s.is_encrypted = s.cipher.is_ready();

        if (is_verbose()) {
          verb("[ server ] SEND %zu bytes to %s:%hu", chunk_size,
               s.address.c_str(), s.port);
        }

        m_node->send_to(s.address, s.port, { buf.data(), chunk_size });
      }

      buf.erase(buf.begin(), buf.begin() + chunk_size);
    }
  }

  void udp_server::update_slots(uint64_t delta_msec) {
    for (size_t i = 0; i < m_slots.size(); i++) {
      if (get_state() == server_state::action) {
        m_slots[i].outdate += delta_msec;
      }

      m_slots[i].wait += delta_msec;

      if (m_slots[i].wait >= get_connection_timeout()) {
        m_slots[i].is_connected = false;

        if (is_join_enabled()) {
          if (get_state() == server_state::prepare) {
            m_slots[i].id_actor = id_undefined;
          }
        } else {
          error(__FUNCTION__, "Disconnect.");
          set_connected(false);
        }
      }
    }

    m_ping_clock += delta_msec;

    if (m_ping_clock >= get_ping_timeout()) {

      for (size_t slot = 0; slot < m_slots.size(); slot++) {
        send_event_to( //
            slot,      //
            encode<ping_request>(get_local_time()));
      }

      m_ping_clock = 0;
    }
  }

  void udp_server::update_local_time(uint64_t delta_msec) {
    const auto t = m_local_time;
    m_local_time += delta_msec;

    if (t > m_local_time) {
      error(__FUNCTION__, "Time value overflow.");
    }
  }

  void udp_server::update_time_limit(uint64_t time) {
    if (m_time_limit < time)
      m_time_limit = time;
  }

  auto udp_server::convert_delta(size_t delta_msec) -> uint64_t {
    if (auto sol = get_solver(); sol) {
      const auto time = sol->get_time();

      const auto delta = adjust_delta(delta_msec) +
                         adjust_overtake(time);

      return min(delta, m_time_limit - time);
    }

    return 0;
  }

  auto udp_server::adjust_overtake(uint64_t time) -> uint64_t {
    uint64_t overtake = 0;

    if (get_tick_duration() > 0) {
      auto ping_ticks = get_ping() / get_tick_duration();
      auto time_gape  = m_time_limit - time;

      if (time_gape > ping_ticks)
        overtake = time_gape - ping_ticks;
    }

    return overtake;
  }

  auto udp_server::has_free_slots() const -> bool {
    return m_slots.size() < m_max_slot_count;
  }
}
