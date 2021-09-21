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
#include "../engine/protocol/all.h"
#include "crypto/ecc_rabbit.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace laplace::network {
  namespace access = engine::access;
  using namespace engine::protocol;

  using std::this_thread::sleep_for, std::min, std::max, std::any_of,
      std::find_if, std::make_unique, std::span, std::numeric_limits,
      std::string, std::string_view, std::chrono::milliseconds,
      engine::ptr_impact, engine::prime_impact, engine::seed_type,
      engine::loader, engine::time_undefined, engine::id_undefined,
      engine::encode, crypto::ecc_rabbit;

  const sl::whole udp_server::default_chunk_size        = 2096;
  const sl::whole udp_server::chunk_size_increment      = 512;
  const sl::whole udp_server::chunk_size_limit          = 0x4000;
  const sl::whole udp_server::default_loss_compensation = 4;
  const uint16_t  udp_server::default_max_command_id    = 400;
  const sl::index udp_server::max_index_delta           = 0x1000;

  udp_server::~udp_server() {
    cleanup();
  }

  void udp_server::set_encryption_enabled(bool is_enabled) noexcept {
    m_is_encryption_enabled = is_enabled;
  }

  void udp_server::set_allowed_commands(span_cuint16 commands) {
    m_allowed_commands.assign(commands.begin(), commands.end());
  }

  void udp_server::set_chunk_size(sl::whole size) {
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
      verb_queue(m_queue.events.size(), seq);

      auto &qu = m_queue.events;
      qu.emplace_back(seq.begin(), seq.end());
      prime_impact::set_index(qu.back(), qu.size() - 1);
    }
  }

  void udp_server::tick(sl::time delta_msec) {
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

  auto udp_server::perform_control(sl::index slot, span_cbyte seq)
      -> bool {

    if (request_token::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      if (!m_slots[slot].token.empty()) {
        send_event_to(slot,
                      encode<session_token>(m_slots[slot].token));
      }

      return true;
    }

    if (server_action::scan(seq)) {
      set_state(server_state::action);
      return !is_master();
    }

    if (server_pause::scan(seq)) {
      set_state(server_state::pause);
      return !is_master();
    }

    if (server_reserve::scan(seq)) {
      if (get_state() == server_state::prepare) {
        if (auto w = get_world(); w) {
          const auto count = server_reserve::get_value(seq);

          for (sl::index n = 0; n < count; n++) { w->reserve(n); }
        }

      } else {
        if (is_verbose()) {
          verb("Network: Ignore reserve command.");
        }
      }

      return true;
    }

    if (server_clock::scan(seq)) {
      set_tick_duration(server_clock::get_value(seq));
      return !is_master();
    }

    if (server_seed::scan(seq)) {
      if (get_state() == server_state::prepare) {

        set_random_seed(
            static_cast<seed_type>(server_seed::get_value(seq)));

        return !is_master();

      } else {
        if (is_verbose()) {
          verb("Network: Ignore seed command.");
        }
      }

      return true;
    }

    if (request_events::scan(seq) && slot != slot_host) {
      if (slot >= 0 && slot < m_slots.size()) {
        const auto count    = request_events::get_event_count(seq);
        sl::index  distance = 0;

        for (sl::index i = 0; i < count; i++) {
          auto n = request_events::get_event(seq, i);

          if (n < m_queue.events.size()) {
            distance = max<sl::index>(distance,
                                      m_queue.events.size() - n);
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
      const auto time = ping_request::get_value(seq);
      send_event_to(slot, encode<ping_response>(time));
      return true;
    }

    if (ping_response::scan(seq) && slot != slot_host) {
      const auto ping = get_local_time() -
                        ping_response::get_value(seq);

      set_ping(ping);
      return true;
    }

    return false;
  }

  void udp_server::cleanup() {
    if (m_loader) {
      m_loader.reset();
    }

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

  auto udp_server::get_local_time() const noexcept -> sl::time {
    return m_local_time;
  }

  void udp_server::update_world(sl::time delta_msec) {
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
    if (m_queue.index < 0) {
      return;
    }

    while (m_queue.index < m_queue.events.size()) {
      send_event(m_queue.events[m_queue.index++]);
    }

    send_chunks();
  }

  void udp_server::send_event_to(sl::index slot, span_cbyte seq) {
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

      for (sl::index i = 0; i < m_slots.size(); i++)
        if (m_slots[i].is_exclusive) {
          slot_create::alter(buf, m_slots[i].id_actor);
          append_event(i, buf);
        }

    } else {
      for (sl::index i = 0; i < m_slots.size(); i++)
        if (m_slots[i].is_exclusive) {
          append_event(i, seq);
        }
    }
  }

  void udp_server::append_event(sl::index slot, span_cbyte seq) {
    if (m_slots[slot].is_connected) {
      m_slots[slot].out.emplace_back(seq.begin(), seq.end());
    }
  }

  void udp_server::set_max_slot_count(sl::whole count) {
    m_max_slot_count = count;
  }

  void udp_server::set_master(bool is_master) {
    m_is_master = is_master;
  }

  auto udp_server::is_allowed(sl::index slot,
                              uint16_t  command_id) const -> bool {
    if (command_id >= m_max_command_id) {
      return false;
    }

    if (!m_slots[slot].is_exclusive) {
      return command_id == ids::session_request ||
             command_id == ids::session_response;
    }

    if (m_allowed_commands.empty()) {
      return true;
    }

    return any_of(m_allowed_commands.begin(),
                  m_allowed_commands.end(),
                  [command_id](auto id) { return id == command_id; });
  }

  auto udp_server::is_master() const -> bool {
    return m_is_master;
  }

  auto udp_server::add_slot(std::string_view address, uint16_t port)
      -> sl::index {

    const auto id = m_slots.size();
    m_slots.emplace_back(
        slot_info { .address = string(address), .port = port });
    return id;
  }

  auto udp_server::has_slot(string_view address, uint16_t port) const
      -> bool {

    for (sl::index i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port) {
        return m_slots[i].id_actor != id_undefined;
      }
    }

    return false;
  }

  auto udp_server::find_slot(string_view address, uint16_t port)
      -> sl::index {

    for (sl::index i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port) {
        return i;
      }
    }

    if (!is_master()) {
      error_("Joining is disabled.", __FUNCTION__);
      verb(fmt("  port: %d", (int) port));
      return -1;
    }

    if (!has_free_slots()) {
      error_("No free slots.", __FUNCTION__);
      return -1;
    }

    return add_slot(address, port);
  }

  void udp_server::process_slots() {
    for (sl::index i = 0; i < m_slots.size(); i++) {
      process_queue(i);

      if (m_slots[i].is_exclusive) {
        check_outdate(i);
      }
    }

    clean_slots();
  }

  void udp_server::process_queue(sl::index slot) {
    auto &    qu = m_slots[slot].queue;
    sl::index n  = 0;

    for (; n < qu.events.size(); n++) {
      auto &seq = qu.events[n];
      if (seq.empty())
        break;
      process_event(slot, seq);
    }

    if (n < qu.events.size() && !m_slots[slot].request_flag) {
      auto events = sl::vector<sl::index> {};

      for (sl::index i = n; i < qu.events.size(); i++) {
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

  void udp_server::check_outdate(sl::index slot) {
    if (m_slots[slot].outdate >= get_update_timeout()) {
      if (is_master()) {
        if (auto sol = get_solver(); sol) {
          send_event_to(slot,
                        encode<server_idle>(m_queue.events.size(),
                                            sol->get_time()));
        }
      }

      const auto compensate = min<sl::index>(m_queue.events.size(),
                                             m_loss_compensation);

      for (sl::index i = m_queue.events.size() - compensate;
           i < m_queue.events.size(); i++) {

        send_event_to(slot, m_queue.events[i]);
      }

      m_slots[slot].outdate = 0;
    }
  }

  void udp_server::clean_slots() {
    if (is_master() && get_state() == server_state::prepare) {
      m_slots.erase(std::remove_if(m_slots.begin(), m_slots.end(),
                                   [](const auto &s) {
                                     return !s.is_connected;
                                   }),
                    m_slots.end());
    }
  }

  void udp_server::process_event(sl::index slot, span_cbyte seq) {
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
        if (is_verbose()) {
          verb("Network: Ignore timed event.");
        }
      }
    }
  }

  void udp_server::distribute_event(sl::index slot, span_cbyte seq) {
    if (seq.empty()) {
      error_("No event.", __FUNCTION__);
      return;
    }

    auto buf = vbyte { seq.begin(), seq.end() };

    /*  Set the event actor id.
     */
    if (slot != slot_host)
      prime_impact::set_actor(buf, m_slots[slot].id_actor);

    const sl::index n = m_queue.events.size();

    prime_impact::set_index(buf, n);

    verb_queue(n, seq);

    if (get_state() == server_state::prepare) {
      /*  Perform the event.
       */
      add_instant_event(buf);

    } else {
      if (auto sol = get_solver(); sol) {
        /*  Apply the event.
         */
        auto time = sol->apply(buf);

        /*  Set the event time.
         */
        prime_impact::set_time(buf, time);
      }
    }

    /*  Add the event to the main queue.
     */
    m_queue.events.emplace_back(buf);
  }

  void udp_server::add_instant_event(span_cbyte seq) {
    if (prime_impact::get_id(seq) == ids::server_loading) {

      if (!m_loader) {
        m_loader = make_unique<loader>();
        m_loader->set_world(get_world());
        m_loader->set_factory(get_factory());
      }

      if (!m_instant_events.empty()) {
        for (sl::index i = 0; i < m_instant_events.size(); i++) {
          m_loader->add_task(m_instant_events[i]);
        }

        m_instant_events.clear();
      }

      m_loader->add_task(seq);

    } else {
      m_instant_events.emplace_back(seq.begin(), seq.end());
    }
  }

  void udp_server::perform_event(sl::index slot, span_cbyte seq) {
    update_time_limit(prime_impact::get_time(seq));

    if (auto sol = get_solver(); sol) {
      sol->apply(seq);
    }
  }

  void udp_server::perform_instant_events() {
    auto wor = get_world();
    auto fac = get_factory();

    if (wor && fac) {
      for (sl::index i = 0; i < m_instant_events.size(); i++) {
        auto ev = fac->decode(m_instant_events[i]);
        ev->perform({ *wor, access::sync });
      }
    }

    m_instant_events.clear();
  }

  void udp_server::receive_chunks() {
    if (!m_node) {
      return;
    }

    for (;;) {
      const auto n = m_node->receive_to(m_buffer.data(),
                                        m_buffer.size(), async);

      if (n == 0) {
        if (m_node->is_msgsize()) {

          if (has_slot(m_node->get_remote_address(),
                       m_node->get_remote_port())) {
            if (is_verbose()) {
              verb("Network: Wrong buffer size.");
            }

            inc_buffer_size();
          }

        } else if (m_node->is_connreset()) {
          const auto slot = find_slot(m_node->get_remote_address(),
                                      m_node->get_remote_port());

          if (slot < 0 || slot >= m_slots.size()) {
            continue;
          }

          if (is_verbose()) {
            verb(fmt("Network: Reset connection on slot %d.",
                     (int) slot));
          }

          disconnect(slot);

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

      auto slot = find_slot(m_node->get_remote_address(),
                            m_node->get_remote_port());

      if (slot < 0 || slot >= m_slots.size()) {
        error_("Unable to find slot.", __FUNCTION__);
        continue;
      }

      process_chunk(
          slot, span_cbyte { m_buffer.data(),
                             static_cast<span_cbyte::size_type>(n) });
    }

    for (sl::index slot = 0; slot < m_slots.size(); slot++) {
      receive_from(slot);
    }
  }

  void udp_server::receive_from(sl::index slot) {
    if (slot < 0 || slot >= m_slots.size()) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    auto &s = m_slots[slot];

    if (!s.node) {
      return;
    }

    for (;;) {
      const auto n = s.node->receive_to(m_buffer.data(),
                                        m_buffer.size(), async);

      const auto sender_changed = s.address !=
                                      s.node->get_remote_address() ||
                                  s.port != s.node->get_remote_port();

      if (n == 0) {
        if (s.node->is_msgsize()) {

          if (!sender_changed) {
            if (is_verbose()) {
              verb("Network: Wrong buffer size.");
            }

            inc_buffer_size();
          }

        } else if (m_node->is_connreset()) {

          if (sender_changed) {
            continue;
          }

          if (is_verbose()) {
            verb(fmt("Network: Reset connection on slot %d.",
                     (int) slot));
          }

          disconnect(slot);

          if (!is_master()) {
            set_quit(true);
            m_node.reset();
          }
        }

        break;
      }

      add_bytes_received(n);

      if (sender_changed) {
        if (s.is_exclusive) {
          break;
        }

        s.address = s.node->get_remote_address();
        s.port    = s.node->get_remote_port();
      }

      if (!s.is_exclusive) {
        s.is_exclusive = true;

        send_event_history_to(slot);
      }

      process_chunk(
          slot, span_cbyte { m_buffer.data(),
                             static_cast<span_cbyte::size_type>(n) });
    }
  }

  void udp_server::process_chunk(sl::index slot, span_cbyte chunk) {
    if (slot < 0 || slot > m_slots.size()) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    auto &s = m_slots[slot];

    const auto plain = s.tran.decode(chunk);

    add_bytes_loss(s.tran.get_loss_count());

    if (plain.empty()) {
      return;
    }

    s.in.insert(s.in.end(), plain.begin(), plain.end());

    s.is_connected = true;
    s.request_flag = false;

    s.wait = 0;
  }

  void udp_server::send_event_history_to(sl::index slot) {
    if (slot < 0 || slot >= m_slots.size()) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    for (sl::index i = 0; i < m_queue.index; i++) {
      send_event_to(slot, m_queue.events[i]);
    }
  }

  void udp_server::inc_buffer_size() {
    if (m_buffer.size() < max_chunk_size) {
      m_buffer.resize(m_buffer.size() + chunk_size_increment);
    }
  }

  void udp_server::add_event(sl::index slot, span_cbyte seq) {
    auto  index = prime_impact::get_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == -1) {
      if (!perform_control(slot, seq)) {
        if (is_verbose()) {
          verb(fmt("Network: Ignore unindexed command on slot %zu.",
                   slot));
        }
      }

    } else if (index >= qu.index) {
      const auto n = index - qu.index;

      if (n >= qu.events.size()) {
        if (n - qu.events.size() >= max_index_delta) {
          if (is_verbose()) {
            verb(fmt("Network: Invalid command index on slot %d.",
                     (int) slot));
          }
          return;
        }

        qu.events.resize(n + 1);
      }

      if (qu.events[n].empty()) {
        verb_slot(slot, index, seq);

        qu.events[n].assign(seq.begin(), seq.end());
        m_slots[slot].outdate = 0;
      }
    }
  }

  void udp_server::receive_events() {
    receive_chunks();
    process_buffers();
  }

  void udp_server::process_buffers() {
    for (sl::index slot = 0; slot < m_slots.size(); slot++) {

      for (sl::index i = 0; i < m_slots[slot].in.size(); i++) {
        const auto &ev = m_slots[slot].in[i];
        const auto  id = prime_impact::get_id(ev);

        if (is_allowed(slot, id)) {
          add_event(slot, ev);
        } else {
          if (is_verbose()) {
            const auto s = engine::basic_factory::name_by_id_native(
                id);

            if (s.empty()) {
              verb(fmt("Network: Command '%d' not allowed.",
                       (int) id));
            } else {
              verb(fmt("Network: Command '%s (%d)' not allowed.",
                       s.c_str(), (int) id));
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

        auto plain = sl::vector<span_cbyte> {};

        for (sl::index i = 0; i < s.out.size(); i++) {
          plain.emplace_back(s.out[i].begin(), s.out[i].end());
        }

        const auto chunk = s.is_encrypted ? s.tran.encode(plain)
                                          : s.tran.pack(plain);

        s.out.clear();

        if (!chunk.empty()) {
          s.is_encrypted = s.tran.is_encrypted();

          if (s.is_exclusive && s.node) {
            add_bytes_sent(s.node->send_to(s.address, s.port, chunk));
          } else {
            add_bytes_sent(m_node->send_to(s.address, s.port, chunk));
          }
        }
      }
    }
  }

  void udp_server::disconnect(sl::index slot) {
    if (slot >= 0 && slot < m_slots.size()) {
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
        verb(fmt("Network: Disconnect on slot %d.", (int) slot));
      }

    } else {
      error_("Invalid slot.", __FUNCTION__);
    }
  }

  void udp_server::update_slots(sl::time delta_msec) {
    for (sl::index i = 0; i < m_slots.size(); i++) {
      if (!m_slots[i].is_connected) {
        continue;
      }

      m_slots[i].outdate += delta_msec;
      m_slots[i].wait += delta_msec;

      if (m_slots[i].wait >= get_connection_timeout()) {
        if (is_verbose()) {
          verb(fmt("Network: Connection timeout on slot %d.",
                   (int) i));
        }

        disconnect(i);
      }
    }

    m_ping_clock += delta_msec;

    if (m_ping_clock >= get_ping_timeout()) {

      for (sl::index slot = 0; slot < m_slots.size(); slot++)
        if (m_slots[slot].is_connected &&
            m_slots[slot].is_exclusive) {
          send_event_to(slot, encode<ping_request>(get_local_time()));
        }

      m_ping_clock = 0;
    }
  }

  void udp_server::update_local_time(sl::time delta_msec) {
    const auto t = m_local_time;
    m_local_time += delta_msec;

    if (t > m_local_time) {
      error_("Time value overflow.", __FUNCTION__);

      cleanup();
      set_quit(true);
    }
  }

  void udp_server::update_time_limit(sl::time time) {
    if (time >= 0 && m_time_limit < time)
      m_time_limit = time;
  }

  auto udp_server::convert_delta(sl::time delta_msec) -> sl::time {
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

  auto udp_server::adjust_overtake(sl::time time) -> sl::time {
    auto overtake = sl::time {};

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
    return m_max_slot_count < 0 || m_slots.size() < m_max_slot_count;
  }
}
