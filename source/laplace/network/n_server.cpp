/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "server.h"

#include "../core/string.h"
#include "../core/utils.h"
#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  using std::min, std::max, std::string, std::string_view,
      crypto::ecc_rabbit;

  sl::time const  server::default_update_timeout_msec     = 10;
  sl::time const  server::default_ping_timeout_msec       = 100;
  sl::time const  server::default_connection_timeout_msec = 10000;
  sl::whole const server::chunk_size                      = 4192;
  sl::whole const server::default_loss_compensation       = 4;
  sl::index const server::max_index_delta                 = 0x1000;

  server::~server() noexcept {
    cleanup();
  }

  void server::setup_protocol_interface(
      protocol_interface const &pin) noexcept {
    if (!check_protocol_interface(pin)) {
      error_("Invalid protocol interface.", __FUNCTION__);
      return;
    }

    m_proto = pin;
  }

  void server::setup_execution_interface(
      execution_interface const &ein) noexcept {
    if (!check_execution_interface(ein)) {
      error_("Invalid event interface.", __FUNCTION__);
      return;
    }

    m_exe = ein;
  }

  void server::setup_log_interface(
      log_interface const &lin) noexcept {
    if (!check_log_interface(lin)) {
      error_("Invalid log interface.", __FUNCTION__);
      return;
    }

    m_slots.set_log_interface(lin);
    m_stats.set_log_interface(lin);

    m_log = lin;
  }

  auto server::is_connected() const noexcept -> bool {
    return m_is_connected;
  }

  auto server::is_quit() const noexcept -> bool {
    return m_is_quit;
  }

  void server::set_encryption_enabled(bool is_enabled) noexcept {
    m_is_encryption_enabled = is_enabled;
  }

  void server::set_max_slot_count(sl::whole count) noexcept {
    m_slots.set_max_count(count);
  }

  void server::listen(uint16_t port) noexcept {
    setup();

    m_mode  = server_mode::host;
    m_state = server_state::prepare;

    m_is_connected = true;

    m_node = m_socket_interface->open(port);

    process_event(slot_host, m_proto.encode_server_clock(
                                 m_clock.get_tick_duration()));
    process_event(slot_host,
                  m_proto.encode_server_seed(m_random.get_seed()));
    process_event(slot_host, m_proto.encode(control::server_init));

    perform_instant_events();
  }

  void server::connect(std::string_view host_address,
                       uint16_t         host_port,
                       uint16_t         client_port) noexcept {
    setup();

    m_mode  = server_mode::remote;
    m_state = server_state::prepare;

    m_is_connected = true;

    m_node = m_socket_interface->open(client_port);

    auto slot = m_slots.add(host_address, host_port);

    if (m_is_encryption_enabled) {
      m_slots[slot].tran.setup_cipher<ecc_rabbit>();

      send_event_to(slot, m_proto.encode_session_request(
                              cipher::rabbit,
                              m_slots[slot].tran.get_public_key()));
    } else {
      send_event_to(
          slot, m_proto.encode_session_request(cipher::plain, {}));
    }

    queue(m_proto.encode(control::client_enter));
  }

  void server::queue(span_cbyte seq) noexcept {
    if (seq.empty()) {
      error_("Ignore empty event.", __FUNCTION__);
      return;
    }

    if (m_mode == server_mode::remote) {
      m_log.queue(m_queue.events.size(), seq);

      auto &qu = m_queue.events;
      qu.emplace_back(seq.begin(), seq.end());
      m_proto.set_event_index(qu.back(), qu.size() - 1);

    } else {
      process_event(slot_host, seq);
    }
  }

  void server::tick(sl::time delta_msec) noexcept {
    m_clock.tick(delta_msec);

    if (m_clock.is_overflow()) {
      cleanup();
      return;
    }

    m_stats.tick();

    receive_events();
    process_slots();
    update_slots(delta_msec);

    send_events();

    update_world();
  }

  auto server::get_port() const noexcept -> uint16_t {
    return m_node ? m_node->get_port() : any_port;
  }

  auto server::perform_control(sl::index  slot,
                               span_cbyte seq) noexcept
      -> event_status {
    auto const control_id = m_proto.get_control_id(seq);

    switch (control_id) {
      case control::request_events:
        return do_request_events(slot, seq);
      case control::request_token: return do_request_token(slot);
      case control::session_request:
        return do_session_request(slot, seq);
      case control::session_response:
        return do_session_response(slot, seq);
      case control::session_token: return do_session_token(slot, seq);
      case control::ping_request: return do_ping_request(slot, seq);
      case control::ping_response: return do_ping_response(slot, seq);
      case control::server_heartbeat:
        return do_server_heartbeat(slot, seq);
      case control::server_init: return do_server_init();
      case control::server_loading: return do_server_loading();
      case control::server_action: return do_server_action();
      case control::server_pause: return do_server_pause();
      case control::server_reserve: return do_server_reserve(seq);
      case control::server_clock: return do_server_clock(seq);
      case control::server_seed: return do_server_seed(seq);
      case control::server_quit: return do_server_quit();
      case control::client_enter: return do_client_enter(slot);
      case control::client_leave: return do_client_leave(slot);
      case control::client_ready: return do_client_ready(slot, seq);
    }

    return event_status::proceed;
  }

  auto server::do_request_events(sl::index  slot,
                                 span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    auto const events   = m_proto.decode_request_events(seq);
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
      m_log.print(
          fmt("Loss compensation %d.", (int) m_loss_compensation));
    }

    return event_status::remove;
  }

  auto server::do_request_token(sl::index slot) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (!m_slots[slot].token.empty())
      send_event_to(
          slot, m_proto.encode_session_token(m_slots[slot].token));

    return event_status::remove;
  }
  auto server::do_session_request(sl::index  slot,
                                  span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::remote) {
      m_log.print("Ignore session request.");
      return event_status::remove;
    }

    m_slots[slot].token = m_random.generate_token();
    m_slots[slot].node  = m_socket_interface->open(any_port);

    auto const cipher_id = m_proto.get_cipher_id(seq);

    switch (cipher_id) {
      case cipher::plain:
        send_event_to(
            slot, m_proto.encode_session_response(
                      m_slots[slot].node->get_port(), span_cbyte {}));
        break;

      case cipher::rabbit:
        m_slots[slot].tran.setup_cipher<ecc_rabbit>();
        m_slots[slot].tran.set_remote_key(
            m_proto.decode_public_key(seq));

        send_event_to(slot, m_proto.encode_session_response(
                                m_slots[slot].node->get_port(),
                                m_slots[slot].tran.get_public_key()));
        break;

      default:
        m_log.print(fmt("Unknown cipher on slot %d.", (int) slot));
    }

    return event_status::remove;
  }

  auto server::do_session_response(sl::index  slot,
                                   span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::host) {
      m_log.print("Ignore session response.");
      return event_status::remove;
    }

    auto const key = m_proto.decode_public_key(seq);

    if (!key.empty()) {
      m_slots[slot].tran.set_remote_key(key);

      m_slots[slot].encryption = m_slots[slot].tran.is_encrypted()
                                     ? transfer::encrypted
                                     : transfer::plain;
    }

    m_slots[slot].node = m_socket_interface->open(any_port);
    m_slots[slot].port = m_proto.decode_session_response_port(seq);
    m_slots[slot].is_exclusive = true;

    if (m_token.empty())
      send_event_to(slot, m_proto.encode(control::request_token));
    else
      send_event_to(slot, m_proto.encode_session_token(m_token));

    return event_status::remove;
  }

  auto server::do_session_token(sl::index  slot,
                                span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::remote) {
      auto const token = m_proto.decode_session_token(seq);
      m_token.assign(token.begin(), token.end());
      return event_status::remove;
    }

    auto const token = m_proto.decode_session_token(seq);

    if (token.empty())
      return event_status::remove;

    for (sl::index i = 0; i < m_slots.get_count(); i++) {
      auto &s = m_slots[i];

      if (i == slot || s.token.empty())
        continue;

      bool const equal = [&]() {
        if (s.token.size() != token.size())
          return false;
        for (sl::index k = 0; k < token.size(); k++)
          if (s.token[k] != token[k])
            return false;
        return true;
      }();

      if (!equal)
        continue;

      if (m_slots[slot].id_actor != id_undefined)
        process_event(slot, m_proto.encode(control::slot_remove));

      m_slots[slot].token    = s.token;
      m_slots[slot].id_actor = s.id_actor;

      s.id_actor     = id_undefined;
      s.is_connected = false;

      break;
    }

    return event_status::remove;
  }

  auto server::do_ping_request(sl::index  slot,
                               span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status ::remove;
    }

    send_event_to(slot, m_proto.encode_ping_response(seq));
    return event_status::remove;
  }

  auto server::do_ping_response(sl::index  slot,
                                span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status ::remove;
    }

    m_clock.set_latency(m_clock.get_local_time() -
                        m_proto.decode_ping_response(seq));
    return event_status::remove;
  }

  auto server::do_server_heartbeat(sl::index  slot,
                                   span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::host) {
      m_log.print("Ignore server heartbeat.");
      return event_status::remove;
    }

    auto const [index, time] = m_proto.decode_server_heartbeat(seq);

    if (index >= 0) {
      auto const &qu          = m_slots[slot].queue;
      auto const  event_count = qu.index + qu.events.size();

      if (index > event_count) {
        auto events = sl::vector<sl::index> {};

        for (sl::index n = event_count; n < index; n++) {
          events.emplace_back(n);
          if (events.size() >= m_proto.get_request_events_limit())
            break;
        }

        send_event_to(slot, m_proto.encode_request_events(events));
      }

      m_clock.set_master_time(time);
    }

    return event_status::remove;
  }

  auto server::do_server_init() noexcept -> event_status {
    return event_status::proceed;
  }

  auto server::do_server_loading() noexcept -> event_status {
    return event_status::proceed;
  }

  auto server::do_server_action() noexcept -> event_status {
    m_state = server_state::action;
    return m_mode == server_mode::host ? event_status::proceed
                                       : event_status::remove;
  }

  auto server::do_server_pause() noexcept -> event_status {
    m_state = server_state::pause;
    return m_mode == server_mode::host ? event_status::proceed
                                       : event_status::remove;
  }

  auto server::do_server_reserve(span_cbyte seq) noexcept
      -> event_status {
    if (m_state == server_state::prepare)
      m_exe.do_server_reserve(seq);
    else
      m_log.print("Ignore reserve command.");
    return event_status::remove;
  }

  auto server::do_server_clock(span_cbyte seq) noexcept
      -> event_status {
    m_clock.set_tick_duration(m_proto.decode_server_clock(seq));
    return m_mode == server_mode::host ? event_status::proceed
                                       : event_status::remove;
  }

  auto server::do_server_seed(span_cbyte seq) noexcept
      -> event_status {
    if (m_state != server_state::prepare) {
      m_log.print("Ignore seed command.");
      return event_status::remove;
    }

    m_exe.do_server_seed(seq);
    return m_mode == server_mode::host ? event_status::proceed
                                       : event_status::remove;
  }

  auto server::do_server_quit() noexcept -> event_status {
    if (m_mode == server_mode::host) {
      m_log.print("Ignore server quit.");
      return event_status::remove;
    }

    cleanup();
    m_is_quit = true;

    return event_status::remove;
  }

  auto server::do_client_enter(sl::index slot) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::remote) {
      m_log.print("Ignore client enter.");
      return event_status::remove;
    }

    if (m_slots[slot].id_actor == id_undefined) {
      m_slots[slot].id_actor = m_exe.do_reserve();
      process_event(slot, m_proto.encode(control::slot_create));
    }

    return event_status::remove;
  }

  auto server::do_client_leave(sl::index slot) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::remote) {
      m_log.print("Ignore client leave.");
      return event_status::remove;
    }

    if (m_state == server_state::prepare) {
      process_event(slot, m_proto.encode(control::slot_remove));
      m_slots[slot].id_actor = id_undefined;
    }

    m_slots[slot].is_connected = false;
    return event_status::remove;
  }

  auto server::do_client_ready(sl::index  slot,
                               span_cbyte seq) noexcept
      -> event_status {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return event_status::remove;
    }

    if (m_mode == server_mode::remote) {
      m_log.print("Ignore client ready.");
      return event_status::remove;
    }

    if (m_state == server_state::prepare)
      add_instant_event(seq);
    else
      m_log.print("Ignore ready command.");

    return event_status::remove;
  }

  void server::reset_connection() noexcept {
    if (m_is_connected) {
      if (m_mode == server_mode::remote)
        queue(m_proto.encode(control::client_leave));
      else
        queue(m_proto.encode(control::server_quit));

      send_events();
    }

    m_queue.index = 0;
    m_queue.events.clear();
    m_slots.clear();

    m_is_connected = false;
  }

  void server::setup() noexcept {
    reset_connection();
    m_is_quit = false;
    m_exe.do_setup();
  }

  void server::cleanup() noexcept {
    reset_connection();
    m_exe.do_cleanup();
  }

  void server::update_world() noexcept {
    if (!m_exe.do_wait_loading())
      return;

    perform_instant_events();

    if (m_state == server_state::action)
      m_exe.do_schedule(m_clock.get_time_elapsed());

    if (m_exe.is_desync())
      cleanup();
  }

  void server::send_events() noexcept {
    if (m_queue.index < 0)
      return;

    while (m_queue.index < m_queue.events.size())
      send_event(m_queue.events[m_queue.index++]);

    send_chunks();
  }

  void server::send_event_to(sl::index  slot,
                             span_cbyte seq) noexcept {
    if (m_proto.get_control_id(seq) == control::slot_create) {
      auto buf = vbyte { seq.begin(), seq.end() };
      m_proto.alter_slot_create_flag(buf, m_slots[slot].id_actor);
      append_event(slot, buf);
    } else {
      append_event(slot, seq);
    }
  }

  void server::send_event(span_cbyte seq) noexcept {
    if (m_proto.get_control_id(seq) == control::slot_create) {
      auto buf = vbyte { seq.begin(), seq.end() };

      for (sl::index i = 0; i < m_slots.get_count(); i++)
        if (m_slots[i].is_exclusive) {
          m_proto.alter_slot_create_flag(buf, m_slots[i].id_actor);
          append_event(i, buf);
        }

    } else {
      for (sl::index i = 0; i < m_slots.get_count(); i++)
        if (m_slots[i].is_exclusive)
          append_event(i, seq);
    }
  }

  void server::append_event(sl::index slot, span_cbyte seq) noexcept {
    if (m_slots[slot].is_connected)
      m_slots[slot].out.emplace_back(seq.begin(), seq.end());
  }

  void server::process_slots() noexcept {
    for (sl::index i = 0; i < m_slots.get_count(); i++) {
      process_queue(i);

      if (m_slots[i].is_exclusive)
        check_outdate(i);
    }

    clean_slots();
  }

  void server::process_queue(sl::index slot) noexcept {
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
      auto const limit  = m_proto.get_request_events_limit();

      for (sl::index i = n; i < qu.events.size(); i++) {
        if (qu.events[i].empty())
          events.emplace_back(qu.index + i);
        if (events.size() >= limit)
          break;
      }

      send_event_to(slot, m_proto.encode_request_events(events));

      m_slots[slot].request_flag = true;
    }

    qu.events.erase(qu.events.begin(), qu.events.begin() + n);

    qu.index += n;
  }

  void server::check_outdate(sl::index slot) noexcept {
    if (m_slots[slot].outdate < m_update_timeout)
      return;

    if (m_mode == server_mode::host)
      send_event_to(slot,
                    m_proto.encode_server_heartbeat(
                        { .index = as_index(m_queue.events.size()),
                          .time  = m_clock.get_time() }));

    auto const compensate = min<sl::index>(m_queue.events.size(),
                                           m_loss_compensation);

    for (sl::index i = m_queue.events.size() - compensate;
         i < m_queue.events.size(); i++)
      send_event_to(slot, m_queue.events[i]);

    m_slots[slot].outdate = 0;
  }

  void server::clean_slots() noexcept {
    /*  TODO
     *  Remove disconnected slots in action state also?
     *  Slot can reconnect by secret token.
     */
    if (m_mode != server_mode::remote &&
        m_state == server_state::prepare)
      m_slots.erase_disconnected();
  }

  void server::process_event(sl::index  slot,
                             span_cbyte seq) noexcept {
    if (m_proto.get_event_time(seq) == time_undefined) {
      if (perform_control(slot, seq) == event_status::remove)
        return;
      if (m_mode == server_mode::remote)
        add_instant_event(seq);
      else
        distribute_event(slot, seq);
    } else {
      if (m_mode == server_mode::host)
        perform_event(slot, seq);
      else
        m_log.print("Ignore timed event.");
    }
  }

  void server::distribute_event(sl::index  slot,
                                span_cbyte seq) noexcept {
    if (seq.empty()) {
      error_("No event.", __FUNCTION__);
      return;
    }

    auto buf = vbyte { seq.begin(), seq.end() };

    if (slot != slot_host)
      m_proto.set_event_actor(buf, m_slots[slot].id_actor);

    sl::index const n = m_queue.events.size();

    m_proto.set_event_index(buf, n);

    m_log.queue(n, seq);

    if (m_state == server_state::prepare)
      add_instant_event(buf);
    else
      m_proto.set_event_time(buf, m_exe.do_apply(buf));

    m_queue.events.emplace_back(buf);
  }

  void server::add_instant_event(span_cbyte seq) noexcept {
    if (m_proto.get_control_id(seq) != control::server_loading) {
      m_instant_events.emplace_back(seq.begin(), seq.end());
      return;
    }

    if (!m_instant_events.empty()) {
      for (auto &instant_event : m_instant_events)
        m_exe.do_loading(instant_event);
      m_instant_events.clear();
    }

    m_exe.do_loading(seq);
  }

  void server::perform_event(sl::index  slot,
                             span_cbyte seq) noexcept {
    m_clock.set_master_time(m_proto.get_event_time(seq));
    m_exe.do_apply(seq);
  }

  void server::perform_instant_events() noexcept {
    for (auto &ev : m_instant_events) m_exe.do_perform(ev);
    m_instant_events.clear();
  }

  void server::receive_chunks() noexcept {
    if (!m_node)
      return;

    for (;;) {
      auto const n = m_node->receive(m_buffer, async);

      if (n == 0) {
        if (m_node->is_connreset()) {
          auto const slot = m_slots.find(
              m_node->get_remote_address(), m_node->get_remote_port(),
              m_mode == server_mode::host
                  ? slot_pool::add_if_not_found
                  : slot_pool::find_default);

          if (!m_slots.has(slot))
            continue;

          m_log.print(
              fmt("Reset connection on slot %d.", (int) slot));

          disconnect(slot);

          if (m_mode == server_mode::host)
            continue;

          cleanup();
        }

        break;
      }

      m_stats.add_bytes_received(n);

      auto slot = m_slots.find(
          m_node->get_remote_address(), m_node->get_remote_port(),
          m_mode == server_mode::host ? slot_pool::add_if_not_found
                                      : slot_pool::find_default);

      if (!m_slots.has(slot)) {
        error_("Unable to find slot.", __FUNCTION__);
        continue;
      }

      process_chunk(
          slot, span_cbyte { m_buffer.data(),
                             static_cast<span_cbyte::size_type>(n) });
    }

    for (sl::index slot = 0; slot < m_slots.get_count(); slot++)
      receive_from(slot);
  }

  void server::receive_from(sl::index slot) noexcept {
    if (!m_slots.has(slot)) {
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

          if (m_mode == server_mode::remote)
            cleanup();
        }

        break;
      }

      m_stats.add_bytes_received(n);

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

  void server::process_chunk(sl::index  slot,
                             span_cbyte chunk) noexcept {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    auto &s = m_slots[slot];

    auto const plain = s.tran.decode(chunk);

    m_stats.add_bytes_loss(s.tran.get_loss_count());

    if (plain.empty())
      return;

    s.in.insert(s.in.end(), plain.begin(), plain.end());

    s.is_connected = true;
    s.request_flag = false;

    s.wait = 0;
  }

  void server::send_event_history_to(sl::index slot) noexcept {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    /*  TODO
     *  Check for sent event count.
     */
    for (sl::index i = 0; i < m_queue.index; i++)
      send_event_to(slot, m_queue.events[i]);
  }

  void server::add_event(sl::index slot, span_cbyte seq) noexcept {
    auto  index = m_proto.get_event_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == index_undefined) {
      if (perform_control(slot, seq) == event_status::proceed)
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

  void server::receive_events() noexcept {
    receive_chunks();
    process_buffers();
  }

  void server::process_buffers() noexcept {
    for (sl::index slot = 0; slot < m_slots.get_count(); slot++) {

      for (sl::index i = 0; i < m_slots[slot].in.size(); i++) {
        auto const &ev = m_slots[slot].in[i];

        if (m_proto.is_allowed(ev, m_slots[i].is_exclusive))
          add_event(slot, ev);
        else
          m_log.not_allowed(slot, ev);
      }

      m_slots[slot].in.clear();
    }
  }

  auto server::encode_chunk(sl::index slot) noexcept -> vbyte {
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

    auto chunk = s.tran.encode(plain, s.encryption);

    if (chunk.empty())
      m_log.print(
          fmt("Unable to encode chunk on slot %d.", (int) slot));

    s.out.clear();

    return chunk;
  }

  void server::send_chunk(sl::index slot, span_cbyte chunk) noexcept {
    if (chunk.empty())
      return;
    if (chunk.size() >= chunk_size)
      m_log.print("Chunk size too big.");

    auto &s = m_slots[slot];

    s.encryption = s.tran.is_encrypted() ? transfer::encrypted
                                         : transfer::plain;

    if (s.is_exclusive && s.node)
      m_stats.add_bytes_sent(s.node->send(s.address, s.port, chunk));
    else
      m_stats.add_bytes_sent(m_node->send(s.address, s.port, chunk));
  }

  void server::send_chunks() noexcept {
    if (!m_node)
      return;

    for (sl::index slot = 0; slot < m_slots.get_count(); slot++) {
      if (m_slots[slot].out.empty())
        continue;

      send_chunk(slot, encode_chunk(slot));
    }
  }

  void server::disconnect(sl::index slot) noexcept {
    if (!m_slots.has(slot)) {
      error_("Invalid slot.", __FUNCTION__);
      return;
    }

    m_slots[slot].is_connected = false;

    if (m_mode == server_mode::remote) {
      m_is_connected = false;
    } else if (m_state == server_state::prepare &&
               m_slots[slot].id_actor != id_undefined) {
      process_event(slot, m_proto.encode(control::slot_remove));
      m_slots[slot].id_actor = id_undefined;
    }

    m_log.print(fmt("Disconnect on slot %d.", (int) slot));
  }

  void server::update_slots(sl::time delta_msec) noexcept {
    for (sl::index i = 0; i < m_slots.get_count(); i++) {
      if (!m_slots[i].is_connected)
        continue;

      m_slots[i].outdate += delta_msec;
      m_slots[i].wait += delta_msec;

      if (m_slots[i].wait >= m_connection_timeout) {
        m_log.print(fmt("Connection timeout on slot %d.", (int) i));
        disconnect(i);
      }
    }

    m_ping_clock += delta_msec;

    if (m_ping_clock >= m_ping_timeout) {

      for (sl::index slot = 0; slot < m_slots.get_count(); slot++)
        if (m_slots[slot].is_connected &&
            m_slots[slot].is_exclusive) {
          send_event_to(slot, m_proto.encode_ping_request(
                                  m_clock.get_local_time()));
        }

      m_ping_clock = 0;
    }
  }
}
