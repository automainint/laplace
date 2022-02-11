/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "udp_server.h"

#include "../core/string.h"
#include "udp_ipv4.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace laplace::network {
  using std::min, std::max, std::any_of, std::make_unique,
      std::string, std::string_view, std::chrono::milliseconds;

  sl::whole const udp_server::chunk_size                = 4192;
  sl::whole const udp_server::default_loss_compensation = 4;
  sl::index const udp_server::max_index_delta           = 0x1000;

  udp_server::udp_server() noexcept {
    m_socket_interface = make_unique<udp_ipv4>();
  }

  udp_server::~udp_server() noexcept {
    cleanup();
  }

  void udp_server::set_encryption_enabled(bool is_enabled) noexcept {
    m_is_encryption_enabled = is_enabled;
  }

  void udp_server::set_allowed_commands(
      span_cuint16 commands) noexcept {
    m_allowed_commands.assign(commands.begin(), commands.end());
  }

  void udp_server::queue(span_cbyte seq) {
    if (seq.empty()) {
      error_("Ignore empty event.", __FUNCTION__);
      return;
    }

    if (is_master()) {
      process_event(slot_host, seq);

    } else {
      m_log.queue(m_queue.events.size(), seq);

      auto &qu = m_queue.events;
      qu.emplace_back(seq.begin(), seq.end());
      m_interface.set_event_index(qu.back(), qu.size() - 1);
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

    if (m_interface.scan_request_token(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      if (!m_slots[slot].token.empty())
        send_event_to(slot, m_interface.encode_session_token(
                                m_slots[slot].token));

      return true;
    }

    if (m_interface.scan_server_action(seq)) {
      set_state(server_state::action);
      return !is_master();
    }

    if (m_interface.scan_server_pause(seq)) {
      set_state(server_state::pause);
      return !is_master();
    }

    if (m_interface.scan_server_reserve(seq)) {
      if (get_state() == server_state::prepare)
        m_interface.do_server_reserve(seq);
      else
        m_log.print("Ignore reserve command.");

      return true;
    }

    if (m_interface.scan_server_clock(seq)) {
      set_tick_duration(m_interface.decode_server_clock(seq));
      return !is_master();
    }

    if (m_interface.scan_server_seed(seq)) {
      if (get_state() == server_state::prepare) {
        m_interface.do_server_seed(seq);
        return !is_master();
      }

      m_log.print("Ignore seed command.");
      return true;
    }

    if (m_interface.scan_request_events(seq) && slot != slot_host) {
      if (slot >= 0 && slot < m_slots.size()) {
        auto const events   = m_interface.decode_request_events(seq);
        sl::index  distance = 0;

        for (auto const n : events) {
          if (n >= 0 && n < m_queue.events.size()) {
            distance = max<sl::index>(distance,
                                      m_queue.events.size() - n);
            send_event_to(slot, m_queue.events[n]);
          } else {
            m_log.print(fmt("No requested event %d.", (int) n));
          }
        }

        if (m_loss_compensation < distance) {
          m_loss_compensation++;
          m_log.print(fmt("Loss compensation %d.",
                          (int) m_loss_compensation));
        }

      } else {
        error_("Invalid slot.", __FUNCTION__);
      }

      return true;
    }

    if (m_interface.scan_ping_request(seq) && slot != slot_host) {
      send_event_to(slot, m_interface.encode_ping_response(seq));
      return true;
    }

    if (m_interface.scan_ping_response(seq) && slot != slot_host) {
      set_ping(get_local_time() -
               m_interface.decode_ping_response(seq));
      return true;
    }

    return false;
  }

  void udp_server::cleanup() {
    m_interface.do_cleanup();

    if (m_node) {
      if (is_master())
        queue(m_interface.encode_server_quit());
      else
        queue(m_interface.encode_client_leave());

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
    if (!m_interface.do_wait_loading())
      return;

    perform_instant_events();

    if (get_state() == server_state::action)
      m_interface.do_schedule(is_master()
                                  ? adjust_delta(delta_msec)
                                  : convert_delta(delta_msec));

    if (m_interface.is_desync())
      set_quit(true);
  }

  void udp_server::send_events() {
    if (m_queue.index < 0)
      return;

    while (m_queue.index < m_queue.events.size())
      send_event(m_queue.events[m_queue.index++]);

    send_chunks();
  }

  void udp_server::send_event_to(sl::index slot, span_cbyte seq) {
    if (m_interface.scan_slot_create(seq)) {
      auto buf = vbyte { seq.begin(), seq.end() };
      m_interface.alter_slot_create_flag(buf, m_slots[slot].id_actor);
      append_event(slot, buf);
    } else {
      append_event(slot, seq);
    }
  }

  void udp_server::send_event(span_cbyte seq) {
    if (m_interface.scan_slot_create(seq)) {
      auto buf = vbyte { seq.begin(), seq.end() };

      for (sl::index i = 0; i < m_slots.size(); i++)
        if (m_slots[i].is_exclusive) {
          m_interface.alter_slot_create_flag(buf,
                                             m_slots[i].id_actor);
          append_event(i, buf);
        }

    } else {
      for (sl::index i = 0; i < m_slots.size(); i++)
        if (m_slots[i].is_exclusive)
          append_event(i, seq);
    }
  }

  void udp_server::append_event(sl::index slot, span_cbyte seq) {
    if (m_slots[slot].is_connected)
      m_slots[slot].out.emplace_back(seq.begin(), seq.end());
  }

  void udp_server::set_max_slot_count(sl::whole count) {
    m_max_slot_count = count;
  }

  void udp_server::set_master(bool is_master) {
    m_is_master = is_master;
  }

  auto udp_server::is_master() const -> bool {
    return m_is_master;
  }

  auto udp_server::add_slot(std::string_view address, uint16_t port)
      -> sl::index {

    auto const id = m_slots.size();
    m_slots.emplace_back(
        slot_info { .address = string(address), .port = port });
    return id;
  }

  auto udp_server::has_slot(string_view address, uint16_t port) const
      -> bool {

    for (auto const &s : m_slots)
      if (s.address == address && s.port == port)
        return s.id_actor != id_undefined;

    return false;
  }

  auto udp_server::find_slot(string_view address, uint16_t port)
      -> sl::index {

    for (sl::index i = 0; i < m_slots.size(); i++)
      if (m_slots[i].address == address && m_slots[i].port == port)
        return i;

    if (!is_master()) {
      m_log.print(fmt("Reject %s:%d. Joining is disabled.",
                      address.data(), (int) port));
      return index_undefined;
    }

    if (!has_free_slots()) {
      m_log.print(fmt("Reject %s:%d. No free slots.", address.data(),
                      (int) port));
      return index_undefined;
    }

    return add_slot(address, port);
  }

  void udp_server::process_slots() {
    for (sl::index i = 0; i < m_slots.size(); i++) {
      process_queue(i);

      if (m_slots[i].is_exclusive)
        check_outdate(i);
    }

    clean_slots();
  }

  void udp_server::process_queue(sl::index slot) {
    auto &qu = m_slots[slot].queue;
    auto  n  = sl::index {};

    for (; n < qu.events.size(); n++) {
      auto &seq = qu.events[n];
      if (seq.empty())
        break;
      process_event(slot, seq);
    }

    /*  TODO
     *  Make sure to request the rest events afterwards.
     */
    if (n < qu.events.size() && !m_slots[slot].request_flag) {
      auto       events = sl::vector<sl::index> {};
      auto const limit  = m_interface.get_request_events_limit();

      for (sl::index i = n; i < qu.events.size(); i++) {
        if (qu.events[i].empty())
          events.emplace_back(qu.index + i);
        if (events.size() >= limit)
          break;
      }

      send_event_to(slot, m_interface.encode_request_events(events));

      m_slots[slot].request_flag = true;
    }

    qu.events.erase(qu.events.begin(), qu.events.begin() + n);

    qu.index += n;
  }

  void udp_server::check_outdate(sl::index slot) {
    if (m_slots[slot].outdate < get_update_timeout())
      return;

    if (is_master())
      send_event_to(slot, m_interface.encode_server_idle(
                              m_queue.events.size()));

    auto const compensate = min<sl::index>(m_queue.events.size(),
                                           m_loss_compensation);

    for (sl::index i = m_queue.events.size() - compensate;
         i < m_queue.events.size(); i++)
      send_event_to(slot, m_queue.events[i]);

    m_slots[slot].outdate = 0;
  }

  void udp_server::clean_slots() {
    /*  TODO
     *  Remove disconnected slots in action state also?
     *  Slot can reconnect by secret token.
     */
    if (!is_master() || get_state() != server_state::prepare)
      return;

    m_slots.erase(
        std::remove_if(m_slots.begin(), m_slots.end(),
                       [](auto const &s) { return !s.is_connected; }),
        m_slots.end());
  }

  void udp_server::process_event(sl::index slot, span_cbyte seq) {
    if (m_interface.get_event_time(seq) == time_undefined) {
      if (perform_control(slot, seq))
        return;
      if (is_master())
        distribute_event(slot, seq);
      else
        add_instant_event(seq);
    } else {
      if (!is_master())
        perform_event(slot, seq);
      else
        m_log.print("Ignore timed event.");
    }
  }

  void udp_server::distribute_event(sl::index slot, span_cbyte seq) {
    if (seq.empty()) {
      error_("No event.", __FUNCTION__);
      return;
    }

    auto buf = vbyte { seq.begin(), seq.end() };

    if (slot != slot_host)
      m_interface.set_event_actor(buf, m_slots[slot].id_actor);

    sl::index const n = m_queue.events.size();

    m_interface.set_event_index(buf, n);

    m_log.queue(n, seq);

    if (get_state() == server_state::prepare)
      add_instant_event(buf);
    else
      m_interface.set_event_time(buf, m_interface.do_apply(buf));

    m_queue.events.emplace_back(buf);
  }

  void udp_server::add_instant_event(span_cbyte seq) {
    if (!m_interface.scan_server_loading(seq)) {
      m_instant_events.emplace_back(seq.begin(), seq.end());
      return;
    }

    if (!m_instant_events.empty()) {
      for (sl::index i = 0; i < m_instant_events.size(); i++)
        m_interface.do_loading(m_instant_events[i]);
      m_instant_events.clear();
    }

    m_interface.do_loading(seq);
  }

  void udp_server::perform_event(sl::index slot, span_cbyte seq) {
    update_time_limit(m_interface.get_event_time(seq));
    m_interface.do_apply(seq);
  }

  void udp_server::perform_instant_events() {
    for (auto &ev : m_instant_events) m_interface.do_perform(ev);
    m_instant_events.clear();
  }

  void udp_server::receive_chunks() {
    if (!m_node)
      return;

    for (;;) {
      auto const n = m_node->receive(m_buffer, async);

      if (n == 0) {
        if (m_node->is_connreset()) {
          auto const slot = find_slot(m_node->get_remote_address(),
                                      m_node->get_remote_port());

          if (slot < 0 || slot >= m_slots.size())
            continue;

          m_log.print(
              fmt("Reset connection on slot %d.", (int) slot));

          disconnect(slot);

          if (is_master())
            continue;

          set_quit(true);
          m_node.reset();
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

    for (sl::index slot = 0; slot < m_slots.size(); slot++)
      receive_from(slot);
  }

  void udp_server::receive_from(sl::index slot) {
    if (slot < 0 || slot >= m_slots.size()) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    auto &s = m_slots[slot];

    if (!s.node)
      return;

    for (;;) {
      auto const n = s.node->receive(m_buffer, async);

      bool const sender_changed = s.address !=
                                      s.node->get_remote_address() ||
                                  s.port != s.node->get_remote_port();

      if (n == 0) {
        if (m_node->is_connreset()) {

          if (sender_changed)
            continue;

          m_log.print(
              fmt("Reset connection on slot %d.", (int) slot));

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
        if (s.is_exclusive)
          break;

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

    auto const plain = s.tran.decode(chunk);

    add_bytes_loss(s.tran.get_loss_count());

    if (plain.empty())
      return;

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

    /*  TODO
     *  Check for sent event count.
     */
    for (sl::index i = 0; i < m_queue.index; i++)
      send_event_to(slot, m_queue.events[i]);
  }

  void udp_server::add_event(sl::index slot, span_cbyte seq) {
    auto  index = m_interface.get_event_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == index_undefined) {
      if (!perform_control(slot, seq))
        m_log.print(
            fmt("Ignore unindexed command on slot %d.", (int) slot));
      return;
    }

    if (index < qu.index) {
      m_log.print(
          fmt("Ignore command duplicate on slot %d.", (int) slot));
      return;
    }

    auto const n = index - qu.index;

    if (n >= qu.events.size()) {
      /*  TODO
       *  Index delta limit may be error prone.
       */
      if (n - qu.events.size() >= max_index_delta) {
        m_log.print(
            fmt("Invalid command index on slot %d.", (int) slot));
        return;
      }

      qu.events.resize(n + 1);
    }

    if (qu.events[n].empty()) {
      m_log.slot(slot, index, seq);

      qu.events[n].assign(seq.begin(), seq.end());
      m_slots[slot].outdate = 0;
    }
  }

  void udp_server::receive_events() {
    receive_chunks();
    process_buffers();
  }

  void udp_server::process_buffers() {
    for (sl::index slot = 0; slot < m_slots.size(); slot++) {

      for (sl::index i = 0; i < m_slots[slot].in.size(); i++) {
        auto const &ev = m_slots[slot].in[i];

        if (m_interface.is_allowed(ev, m_slots[i].is_exclusive))
          add_event(slot, ev);
        else
          m_log.not_allowed(slot, ev);
      }

      m_slots[slot].in.clear();
    }
  }

  auto udp_server::encode_chunk(sl::index slot) noexcept -> vbyte {
    auto &s = m_slots[slot];

    auto plain = sl::vector<span_cbyte> {};
    auto size  = sl::whole {};

    for (sl::index i = 0; i < s.out.size(); i++) {
      size += s.tran.get_data_overhead();
      size += s.out[i].size();

      if (size >= chunk_size) {
        m_log.print(
            fmt("Some events discarded on slot %d.", (int) slot));
        break;
      }

      plain.emplace_back<span_cbyte>(
          { s.out[i].begin(), s.out[i].end() });
    }

    auto const chunk = s.tran.encode(plain, s.encryption);

    if (chunk.empty())
      m_log.print(
          fmt("Unable to encode chunk on slot %d.", (int) slot));

    s.out.clear();

    return chunk;
  }

  void udp_server::send_chunk(sl::index  slot,
                              span_cbyte chunk) noexcept {
    if (chunk.empty())
      return;
    if (chunk.size() >= get_max_chunk_size())
      m_log.print("Warning: Chunk size too big.");

    auto &s = m_slots[slot];

    s.encryption = s.tran.is_encrypted() ? transfer::encrypted
                                         : transfer::plain;

    if (s.is_exclusive && s.node)
      add_bytes_sent(s.node->send(s.address, s.port, chunk));
    else
      add_bytes_sent(m_node->send(s.address, s.port, chunk));
  }

  void udp_server::send_chunks() {
    if (!m_node)
      return;

    for (sl::index slot = 0; slot < m_slots.size(); slot++) {
      if (m_slots[slot].out.empty())
        continue;

      send_chunk(slot, encode_chunk(slot));
    }
  }

  void udp_server::disconnect(sl::index slot) {
    if (slot < 0 || slot >= m_slots.size()) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    m_slots[slot].is_connected = false;

    if (is_master()) {
      if (get_state() == server_state::prepare &&
          m_slots[slot].id_actor != id_undefined) {
        process_event(slot, m_interface.encode_slot_remove());
        m_slots[slot].id_actor = id_undefined;
      }
    } else {
      set_connected(false);
    }

    m_log.print(fmt("Disconnect on slot %d.", (int) slot));
  }

  void udp_server::update_slots(sl::time delta_msec) {
    for (sl::index i = 0; i < m_slots.size(); i++) {
      if (!m_slots[i].is_connected)
        continue;

      m_slots[i].outdate += delta_msec;
      m_slots[i].wait += delta_msec;

      if (m_slots[i].wait >= get_connection_timeout()) {
        m_log.print(fmt("Connection timeout on slot %d.", (int) i));
        disconnect(i);
      }
    }

    m_ping_clock += delta_msec;

    if (m_ping_clock >= get_ping_timeout()) {

      for (sl::index slot = 0; slot < m_slots.size(); slot++)
        if (m_slots[slot].is_connected &&
            m_slots[slot].is_exclusive) {
          send_event_to(slot, m_interface.encode_ping_request(
                                  get_local_time()));
        }

      m_ping_clock = 0;
    }
  }

  void udp_server::update_local_time(sl::time delta_msec) {
    auto const t = m_local_time;

    m_local_time = static_cast<sl::time>(
        static_cast<uint64_t>(m_local_time) +
        static_cast<uint64_t>(delta_msec));

    if (m_local_time < t) {
      error_("Time value overflow.", __FUNCTION__);
      set_quit(true);
    }
  }

  void udp_server::update_time_limit(sl::time time) {
    if (time >= 0 && m_time_limit < time)
      m_time_limit = time;
  }

  auto udp_server::convert_delta(sl::time delta_msec) -> sl::time {
    auto const time = m_interface.get_current_time();

    if (time < m_time_limit) {
      auto const delta = adjust_delta(delta_msec) +
                         adjust_overtake(time);

      return min(delta, m_time_limit - time);
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
