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

#include "udp_server.h"

#include "../core/utils.h"
#include "../engine/prime_impact.h"
#include "../engine/protocol/basic_event.h"
#include "../engine/protocol/ids.h"
#include "../engine/protocol/ping.h"
#include "../engine/protocol/public_key.h"
#include "../engine/protocol/request_events.h"
#include "../engine/protocol/server_clock.h"
#include "../engine/protocol/server_idle.h"
#include "../engine/protocol/server_seed.h"
#include "../engine/protocol/slot_create.h"
#include "../engine/protocol/slot_remove.h"
#include "crypto/ecc_rabbit.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace laplace::network {
  namespace access = engine::access;
  namespace pro    = engine::protocol;
  namespace ids    = pro::ids;

  using std::this_thread::sleep_for, std::min, std::max, std::any_of,
      std::find_if, std::make_unique, std::span, std::numeric_limits,
      std::string, std::vector, std::string_view,
      std::chrono::milliseconds, engine::ptr_impact,
      engine::prime_impact, engine::seed_type, engine::loader,
      pro::slot_create, pro::slot_remove, pro::request_events,
      pro::server_idle, pro::ping_request, pro::ping_response,
      pro::public_key, pro::server_action, pro::server_pause,
      pro::server_clock, pro::server_seed, pro::server_quit,
      pro::client_leave, engine::time_undefined, engine::id_undefined,
      engine::encode, crypto::ecc_rabbit;

  udp_server::~udp_server() {
    cleanup();
  }

  void udp_server::set_encryption_enabled(bool is_enabled) noexcept {
    m_is_encryption_enabled = is_enabled;
  }

  void udp_server::set_allowed_commands(span_cuint16 commands) {
    m_allowed_commands.assign(commands.begin(), commands.end());
  }

  void udp_server::set_chunk_size(size_t size) {
    m_buffer.resize(size);
  }

  void udp_server::queue(span_cbyte seq) {
    if (seq.empty()) {
      error_("Ignore empty event.", __FUNCTION__);
      return;
    }

    if (is_master()) {
      process_event(slot_host, seq);

    } else {
      if (is_verbose()) {
        const auto id    = prime_impact::get_id(seq);
        const auto index = m_queue.events.size();

        if (id < ids::_native_count) {
          verb(fmt(" :: queue %4zu '%s (%hu)'", index,
                   ids::table[id].data(), id));
        } else {
          verb(fmt(" :: queue %4zu '%hu'", index, id));
        }
      }

      auto &qu = m_queue.events;
      qu.emplace_back(vbyte { seq.begin(), seq.end() });
      prime_impact::set_index(qu.back(), qu.size() - 1);
    }
  }

  void udp_server::tick(uint64_t delta_msec) {
    reset_tick();

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

  auto udp_server::perform_control(size_t slot, span_cbyte seq)
      -> bool {

    if (server_action::scan(seq)) {
      set_state(server_state::action);
      return !is_master();
    }

    if (server_pause::scan(seq)) {
      set_state(server_state::pause);
      return !is_master();
    }

    if (server_clock::scan(seq)) {
      set_tick_duration(server_clock::get_tick_duration(seq));
      return !is_master();
    }

    if (server_seed::scan(seq)) {
      if (get_state() == server_state::prepare) {

        set_random_seed(            //
            static_cast<seed_type>( //
                server_seed::get_seed(seq)));

        return !is_master();

      } else {
        /*  It makes no sense to change the seed
         *  after preparation.
         */
        verb("Network: Ignore seed command.");
      }

      return true;
    }

    if (request_events::scan(seq) && slot != slot_host) {
      if (slot < m_slots.size()) {
        const auto count    = request_events::get_event_count(seq);
        size_t     distance = 0;

        for (size_t i = 0; i < count; i++) {
          auto n = request_events::get_event(seq, i);

          if (n < m_queue.events.size()) {
            distance = max(distance, m_queue.events.size() - n);
            send_event_to(slot, m_queue.events[n]);
          } else if (is_verbose()) {
            verb(fmt("Network: No requested event %zu.", n));
          }
        }

        if (m_loss_compensation < distance) {
          m_loss_compensation++;

          if (is_verbose()) {
            verb(fmt("Network: Loss compensation %zu.",
                     m_loss_compensation));
          }
        }

      } else {
        error_("Invalid slot.", __FUNCTION__);
      }

      return true;
    }

    if (ping_request::scan(seq) && slot != slot_host) {
      const auto time = ping_request::get_ping_time(seq);
      send_event_to(slot, encode<ping_response>(time));
      return true;
    }

    if (ping_response::scan(seq) && slot != slot_host) {
      const auto ping = get_local_time() -
                        ping_response::get_ping_time(seq);

      set_ping(ping);
      return true;
    }

    if (public_key::scan(seq) && slot != slot_host) {
      const auto cipher_id = public_key::get_cipher(seq);

      if (cipher_id == ids::cipher_ecc_rabbit) {

        if (slot < m_slots.size()) {
          if (is_master()) {

            if (!m_slots[slot].tran.is_encrypted()) {
              m_slots[slot].tran.setup_cipher<ecc_rabbit>();
            }

            send_event_to(                  //
                slot,                       //
                encode<public_key>(         //
                    ids::cipher_ecc_rabbit, //
                    m_slots[slot].tran.get_public_key()));
          }

          if (!m_slots[slot].tran.is_encrypted()) {
            m_slots[slot].is_encrypted = !is_master();
            m_slots[slot].tran.set_remote_key(public_key::get_key(seq));

            if (is_verbose()) {
              verb("Network: Mutual key");
            }

            dump(m_slots[slot].tran.get_mutual_key());
          }

        } else {
          error_("Invalid slot.", __FUNCTION__);
        }

      } else if (cipher_id != ids::cipher_plain) {
        verb("Network: Unknown cipher.");
      }

      return true;
    }

    return false;
  }

  void udp_server::cleanup() {
    if (m_node) {
      if (is_master()) {
        emit<server_quit>();
      } else {
        emit<client_leave>();
      }

      send_events();
    }

    m_queue.index = 0;
    m_queue.events.clear();
    m_slots.clear();

    m_time_limit = 0;

    set_connected(false);
  }

  auto udp_server::is_encryption_enabled() const noexcept -> bool {
    return m_is_encryption_enabled;
  }

  auto udp_server::get_local_time() const noexcept -> uint64_t {
    return m_local_time;
  }

  void udp_server::update_world(uint64_t delta_msec) {
    if (m_loader && m_loader->is_ready()) {
      m_loader.reset();
    }

    if (!m_loader) {
      perform_instant_events();

      if (is_master()) {
        if (get_state() == server_state::action) {
          if (auto sol = get_solver(); sol) {
            sol->schedule(adjust_delta(delta_msec));
          } else {
            error_("No solver.", __FUNCTION__);
            set_state(server_state::pause);
          }
        }
      } else {
        if (get_state() == server_state::action) {
          if (auto sol = get_solver(); sol) {
            sol->schedule(convert_delta(delta_msec));
          } else {
            error_("No solver.", __FUNCTION__);
            set_state(server_state::pause);
          }
        }
      }

      if (auto wor = get_world(); wor) {
        if (wor->is_desync()) {
          cleanup();
          set_quit(true);
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

  void udp_server::send_event_to(size_t slot, span_cbyte seq) {
    if (slot_create::scan(seq)) {
      auto buf = vbyte(seq.begin(), seq.end());
      slot_create::alter(buf, m_slots[slot].id_actor);

      append_event(slot, buf);
    } else {
      append_event(slot, seq);
    }
  }

  void udp_server::send_event(span_cbyte seq) {
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

  void udp_server::append_event(size_t slot, span_cbyte seq) {
    if (m_slots[slot].is_connected) {
      m_slots[slot].out.emplace_back(vbyte { seq.begin(), seq.end() });
    }
  }

  void udp_server::set_max_slot_count(size_t count) {
    m_max_slot_count = count;
  }

  void udp_server::set_master(bool is_master) {
    m_is_master = is_master;
  }

  auto udp_server::is_allowed(size_t command_id) const -> bool {
    return m_allowed_commands.empty() ||
           any_of(m_allowed_commands.begin(),
                  m_allowed_commands.end(), [command_id](auto id) {
                    return id == command_id;
                  });
  }

  auto udp_server::is_master() const -> bool {
    return m_is_master;
  }

  auto udp_server::add_slot(std::string_view address, uint16_t port)
      -> size_t {

    const auto id = m_slots.size();
    m_slots.emplace_back(
        slot_info { .address = string(address), .port = port });
    return id;
  }

  auto udp_server::has_slot(string_view address, uint16_t port) const
      -> bool {

    for (size_t i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port) {
        return m_slots[i].id_actor != id_undefined;
      }
    }

    return false;
  }

  auto udp_server::find_slot(string_view address, uint16_t port)
      -> size_t {

    for (size_t i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port) {
        return i;
      }
    }

    if (!is_master()) {
      error_("Joining is disabled.", __FUNCTION__);
      return -1;
    }

    if (!has_free_slots()) {
      error_("No free slots.", __FUNCTION__);
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

      send_event_to(slot, encode<request_events>(events));

      m_slots[slot].request_flag = true;
    }

    qu.events.erase(qu.events.begin(),
                    qu.events.begin() + static_cast<ptrdiff_t>(n));

    qu.index += n;
  }

  void udp_server::check_outdate(size_t slot) {
    if (m_slots[slot].outdate >= get_update_timeout()) {
      if (is_master()) {
        if (auto sol = get_solver(); sol) {
          send_event_to(                 //
              slot,                      //
              encode<server_idle>(       //
                  m_queue.events.size(), //
                  sol->get_time()));
        }
      }

      if (is_encryption_enabled() && !m_slots[slot].is_encrypted) {
        const auto key = m_slots[slot].tran.get_public_key();

        if (!key.empty()) {
          send_event_to(                  //
              slot,                       //
              encode<public_key>(         //
                  ids::cipher_ecc_rabbit, //
                  key));
        }
      }

      const auto compensate = min(
          m_queue.events.size(), m_loss_compensation);

      for (size_t i = m_queue.events.size() - compensate;
           i < m_queue.events.size(); i++) {

        send_event_to(slot, m_queue.events[i]);
      }

      m_slots[slot].outdate = 0;
    }
  }

  void udp_server::clean_slots() {
    if (is_master()) {
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

  void udp_server::process_event(size_t slot, span_cbyte seq) {
    if (prime_impact::get_time(seq) == time_undefined) {
      if (perform_control(slot, seq))
        return;

      if (is_master()) {
        distribute_event(slot, seq);
      } else {
        add_instant_event(seq);
      }

    } else {
      if (!is_master()) {
        perform_event(slot, seq);
      } else {
        verb("Network: Ignore timed event.");
      }
    }
  }

  void udp_server::distribute_event(size_t slot, span_cbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("Network: Unable to decode command.");
        add_bytes_loss(seq.size() + transfer::get_data_overhead());
        return;
      }

      /*  Set the event actor id.
       */
      if (slot != slot_host)
        ev->set_actor(m_slots[slot].id_actor);

      ev->set_index(m_queue.events.size());

      if (is_verbose()) {
        const auto id    = prime_impact::get_id(seq);
        const auto index = m_queue.events.size();

        if (id < ids::_native_count) {
          verb(fmt(" :: queue %4zu '%s (%hu)'", index,
                   ids::table[id].data(), id));
        } else {
          verb(fmt(" :: queue %4zu '%hu'", index, id));
        }
      }

      if (get_state() == server_state::prepare) {
        /*  Perform the event. The event
         *  time will be undefined.
         */
        add_instant_event(prime_impact::get_id(seq), ev);
      } else {
        if (auto sol = get_solver(); sol) {
          /*  Apply the event and
           *  set the event time.
           */
          sol->apply(ev);
        }
      }

      /*  Add the event to the main queue.
       */
      m_queue.events.emplace_back(ev->encode());

    } else {
      error_("No factory.", __FUNCTION__);
    }
  }

  void udp_server::add_instant_event(uint16_t id, ptr_impact ev) {
    if (id == ids::server_loading) {

      if (!m_loader) {
        m_loader = make_unique<loader>();
        m_loader->set_world(get_world());
      }

      if (!m_instant_events.empty()) {
        for (size_t i = 0; i < m_instant_events.size(); i++)
          m_loader->add_task(m_instant_events[i]);

        m_instant_events.clear();
      }

      m_loader->add_task(ev);

    } else {
      m_instant_events.emplace_back(ev);
    }
  }

  void udp_server::add_instant_event(span_cbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("Network: Unable to decode command.");
        return;
      }

      add_instant_event(prime_impact::get_id(seq), ev);

    } else {
      error_("No factory.", __FUNCTION__);
    }
  }

  void udp_server::perform_event(size_t slot, span_cbyte seq) {
    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("Network: Unable to decode command.");
        return;
      }

      update_time_limit(ev->get_time());

      if (auto sol = get_solver(); sol) {
        sol->apply(ev);
      }
    }
  }

  void udp_server::perform_instant_events() {
    if (auto wor = get_world(); wor) {
      for (size_t i = 0; i < m_instant_events.size(); i++) {
        m_instant_events[i]->perform({ *wor, access::sync });
      }

      m_instant_events.clear();
    }
  }

  void udp_server::receive_chunks() {
    if (m_node) {
      for (;;) {
        const auto n = m_node->receive_to(
            m_buffer.data(), m_buffer.size(), async);

        if (n == 0) {
          if (m_node->is_msgsize()) {

            if (has_slot(m_node->get_remote_address(),
                         m_node->get_remote_port())) {
              if (m_buffer.size() < max_chunk_size)
                m_buffer.resize(m_buffer.size() + chunk_size_increment);
            }

          } else if (m_node->is_connreset()) {
            disconnect(find_slot(             //
                m_node->get_remote_address(), //
                m_node->get_remote_port()));

            if (is_master()) {
              continue;
            } else {
              set_quit(true);
              m_node.reset();
            }
          }

          break;
        }

        add_bytes_received(n);

        auto slot = find_slot(            //
            m_node->get_remote_address(), //
            m_node->get_remote_port());

        if (slot >= m_slots.size()) {
          error_("Unable to find slot.", __FUNCTION__);
          continue;
        }

        auto plain = m_slots[slot].tran.decode({ m_buffer.data(), n });

        add_bytes_loss(m_slots[slot].tran.get_loss_count());

        if (plain.empty()) {
          continue;
        }

        m_slots[slot].in.insert(    //
            m_slots[slot].in.end(), //
            plain.begin(),          //
            plain.end());

        m_slots[slot].is_connected = true;
        m_slots[slot].request_flag = false;

        m_slots[slot].wait = 0;
      }
    }
  }

  void udp_server::add_event(size_t slot, span_cbyte seq) {
    auto  index = prime_impact::get_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == -1) {
      /*  For public_key, request_events and ping.
       */
      if (!perform_control(slot, seq)) {
        verb(fmt(
            "Network: Ignore unindexed command on slot %zu.", slot));
      }

    } else if (index >= qu.index) {
      const auto n = index - qu.index;

      if (n < qu.events.size() + max_index_delta) {
        if (n >= qu.events.size()) {
          qu.events.resize(n + 1);
        }

        if (qu.events[n].empty()) {
          if (is_verbose()) {
            const auto id = prime_impact::get_id(seq);

            if (id < ids::_native_count) {
              verb(fmt(" :: (slot %2zu) %4zu '%s (%hu)'", slot, index,
                       ids::table[id].data(), id));
            } else {
              verb(fmt(" :: (slot %2zu) %4zu '%hu'", slot, index, id));
            }
          }

          qu.events[n].assign(seq.begin(), seq.end());
          m_slots[slot].outdate = 0;
        }
      } else {
        verb("Network: Invalid command index.");
        add_bytes_loss(seq.size() + transfer::get_data_overhead());
      }
    }
  }

  void udp_server::receive_events() {
    receive_chunks();
    process_buffers();
  }

  void udp_server::process_buffers() {
    for (size_t slot = 0; slot < m_slots.size(); slot++) {

      for (size_t i = 0; i < m_slots[slot].in.size(); i++) {
        const auto &ev = m_slots[slot].in[i];
        const auto  id = prime_impact::get_id(ev);

        if (is_allowed(id)) {
          add_event(slot, ev);
        } else {
          if (is_verbose()) {
            if (id < ids::_native_count) {
              verb(fmt("Network: Command '%s (%hu)' not allowed.",
                       ids::table[id].data(), id));
            } else {
              verb(fmt("Network: Command '%hu' not allowed.", id));
            }
          }
        }
      }

      m_slots[slot].in.clear();
    }
  }

  void udp_server::send_chunks() {
    if (m_node) {
      for (auto &s : m_slots) {

        if (s.out.empty())
          continue;

        auto plain = vector<span_cbyte> {};

        for (size_t i = 0; i < s.out.size(); i++) {
          plain.emplace_back(
              span_cbyte { s.out[i].begin(), s.out[i].end() });
        }

        const auto chunk = s.is_encrypted ? s.tran.encode(plain)
                                          : s.tran.pack(plain);

        s.out.clear();

        if (!chunk.empty()) {
          s.is_encrypted = s.tran.is_encrypted();
          add_bytes_sent(m_node->send_to(s.address, s.port, chunk));
        }
      }
    }
  }

  void udp_server::disconnect(size_t slot) {
    if (slot < m_slots.size()) {
      m_slots[slot].is_connected = false;

      if (is_master()) {
        if (get_state() == server_state::prepare &&
            m_slots[slot].id_actor != id_undefined) {

          process_event(slot, encode<slot_remove>());
          m_slots[slot].id_actor = id_undefined;
        }
      } else {
        set_connected(false);
      }

      if (is_verbose()) {
        verb(fmt("Network: Disconnect on slot %zu.", slot));
      }

    } else {
      error_("Invalid slot.", __FUNCTION__);
    }
  }

  void udp_server::update_slots(uint64_t delta_msec) {
    for (size_t i = 0; i < m_slots.size(); i++) {
      m_slots[i].outdate += delta_msec;
      m_slots[i].wait += delta_msec;

      if (m_slots[i].is_connected) {
        if (m_slots[i].wait >= get_connection_timeout()) {
          disconnect(i);
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
      error_("Time value overflow.", __FUNCTION__);

      cleanup();
      set_quit(true);
    }
  }

  void udp_server::update_time_limit(uint64_t time) {
    if (time != -1 && m_time_limit < time)
      m_time_limit = time;
  }

  auto udp_server::convert_delta(uint64_t delta_msec) -> uint64_t {
    if (auto sol = get_solver(); sol) {
      const auto time = sol->get_time();

      if (time < m_time_limit) {
        const auto delta = adjust_delta(delta_msec) +
                           adjust_overtake(time);

        return min(delta, m_time_limit - time);
      }
    }

    return 0;
  }

  auto udp_server::adjust_overtake(uint64_t time) -> uint64_t {
    uint64_t overtake = 0;

    if (get_tick_duration() > 0) {
      auto ping_ticks = get_ping() /
                        (get_tick_duration() * get_overtake_factor());

      auto time_gape = m_time_limit - time;

      if (time_gape > ping_ticks)
        overtake = time_gape - ping_ticks;
    }

    return overtake;
  }

  auto udp_server::has_free_slots() const -> bool {
    return m_slots.size() < m_max_slot_count;
  }
}
