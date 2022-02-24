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

#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  using std::min, std::max, std::string, std::string_view,
      crypto::ecc_rabbit, std::span, std::unique_ptr,
      std::default_sentinel, std::move;

  server::server(server &&other) noexcept :
      m_proto(move(other.m_proto)), m_exe(move(other.m_exe)),
      m_endpoints(move(other.m_endpoints)),
      m_random(move(other.m_random)), m_tran(move(other.m_tran)),
      m_is_connected(other.m_is_connected),
      m_session_io(move(other.m_session_io)),
      m_session_node(move(other.m_session_node)),
      m_session_tran(move(other.m_session_tran)),
      m_remote_address(move(other.m_remote_address)),
      m_remote_port(other.m_remote_port),
      m_session_token(move(other.m_session_token)),
      m_actor(move(other.m_actor)) {
    other.m_is_connected = false;
  }

  auto server::operator=(server &&other) noexcept -> server & {
    stop();

    m_proto          = move(other.m_proto);
    m_exe            = move(other.m_exe);
    m_endpoints      = move(other.m_endpoints);
    m_random         = move(other.m_random);
    m_tran           = move(other.m_tran);
    m_is_connected   = other.m_is_connected;
    m_session_io     = move(other.m_session_io);
    m_session_node   = move(other.m_session_node);
    m_session_tran   = move(other.m_session_tran);
    m_remote_address = move(other.m_remote_address);
    m_remote_port    = other.m_remote_port;
    m_session_token  = move(other.m_session_token);
    m_actor          = other.m_actor;

    other.m_is_connected = false;
    return *this;
  }

  server::~server() noexcept {
    stop();
  }

  void server::setup_protocol(
      protocol_interface const &interface) noexcept {
    if (!check_protocol_interface(interface)) {
      log(log_event::error, "Invalid protocol interface.",
          __FUNCTION__);
      return;
    }

    m_proto = interface;
  }

  void server::setup_execution(
      execution_interface const &interface) noexcept {
    if (!check_execution_interface(interface)) {
      log(log_event::error, "Invalid event interface.", __FUNCTION__);
      return;
    }

    m_exe = interface;
  }

  void server::enable_encryption(bool is_enabled) noexcept { }

  void server::set_max_slot_count(sl::whole count) noexcept { }

  void server::set_token(span_cbyte token) noexcept {
    m_session_token.assign(token.begin(), token.end());
  }

  auto server::listen(span<endpoint_info const> endpoints) noexcept
      -> coroutine::task<> {
    stop();
    m_is_connected = true;
    m_is_master    = true;

    for (auto &info : endpoints)
      m_endpoints.emplace_back<endpoint>(
          { .io = info.io, .node = info.io->open(info.port) });

    while (is_connected()) {
      co_yield default_sentinel;
      tick();
    }
  }

  auto server::connect(connect_info const &info) noexcept
      -> coroutine::task<> {
    stop();
    m_is_connected = true;
    m_is_master    = false;

    m_endpoints.clear();
    m_endpoints.emplace_back<endpoint>(
        { .io = info.io, .node = info.io->open(info.client_port) });

    m_remote_address = info.host_address;

    send_session_request(m_endpoints[0], info.host_address,
                         info.host_port);

    while (is_connected()) {
      co_yield default_sentinel;
      tick();
    }
  }

  void server::queue(span_cbyte seq) noexcept { }

  void server::time_elapsed(sl::time delta_msec) noexcept { }

  auto server::session_of(sl::index actor_id) const noexcept
      -> session_info {
    if (m_actor != actor_id)
      return {};

    return { .io      = m_session_io,
             .address = m_remote_address,
             .port    = m_remote_port,
             .token   = m_session_token };
  }

  auto server::get_actors() const noexcept -> sl::vector<sl::index> {
    if (m_actor == id_undefined)
      return {};

    return { m_actor };
  }

  auto server::get_port(sl::index io) const noexcept -> uint16_t {
    if (io < 0 || io >= m_endpoints.size()) {
      log(log_event::error, "Invalid IO index.", __FUNCTION__);
      return any_port;
    }

    if (!m_endpoints[io].node)
      return any_port;

    return m_endpoints[io].node->get_port();
  }

  auto server::get_token() const noexcept -> span_cbyte {
    return m_session_token;
  }

  auto server::has_token() const noexcept -> bool {
    return !m_session_token.empty();
  }

  auto server::is_connected() const noexcept -> bool {
    return m_is_connected;
  }

  auto server::is_quit() const noexcept -> bool {
    return false;
  }

  void server::tick() noexcept {
    read_endpoints();
    read_sessions();
  }

  void server::read_endpoints() noexcept {
    for (auto &ep : m_endpoints) read_endpoint(ep);
  }

  void server::read_endpoint(endpoint const &ep) noexcept {
    constexpr sl::whole chunk_size      = 2048;
    uint8_t             buf[chunk_size] = {};

    process_requests(
        ep, m_tran.decode({ buf, buf + ep.node->receive(buf) }));
  }

  void server::process_requests(endpoint const   &ep,
                                span<vbyte const> reqs) noexcept {
    for (auto &req : reqs) process_request(ep, req);
  }

  void server::process_request(endpoint const &ep,
                               span_cbyte      req) noexcept {
    switch (m_proto.get_control_id(req)) {
      case control::session_request:
        session_open(ep);
        setup_cipher(req);
        send_session_response(ep.node);
        break;
      case control::session_response:
        session_open(ep);
        set_remote_endpoint(req);
        set_remote_key(req);
        if (has_token()) {
          send_session_token(m_session_node);
        } else {
          send_client_enter(m_session_node);
          send_request_token(m_session_node);
        }
        break;
      default:;
    }
  }

  void server::read_sessions() noexcept {
    if (!m_session_node)
      return;

    constexpr sl::whole chunk_size      = 2048;
    uint8_t             buf[chunk_size] = {};

    auto const received = m_session_node->receive(buf);

    if (received == 0)
      return;

    auto const reqs = m_session_tran.decode({ buf, buf + received });

    for (auto &req : reqs) process_session(req);
  }

  void server::process_session(span_cbyte req) noexcept {
    if (!m_proto.is_allowed(req))
      return;

    switch (m_proto.get_control_id(req)) {
      case control::request_token:
        m_session_token = m_random.generate_token();
        send_session_token(m_session_node);
        break;
      case control::session_token:
        set_token(m_proto.decode_session_token(req));
        break;
      case control::client_enter:
        create_actor();
        send_events(m_session_node);
        break;
      case control::client_leave: remove_actor(); break;
      default:;
    }
  }

  void server::session_open(endpoint const &ep) noexcept {
    m_session_node = ep.io->open(any_port);

    if (!ep.node)
      return;

    m_remote_address = ep.node->get_remote_address();
    m_remote_port    = ep.node->get_remote_port();
  }

  void server::setup_cipher(span_cbyte req) {
    switch (m_proto.decode_cipher_id(req)) {
      case cipher::rabbit:
        m_session_tran.setup_cipher<ecc_rabbit>();
        m_session_tran.set_remote_key(m_proto.decode_public_key(req));
        break;
      default: break;
    }
  }

  void server::send_session_request(endpoint const &ep,
                                    string_view     address,
                                    uint16_t        port) noexcept {
    m_session_tran.setup_cipher<ecc_rabbit>();
    m_session_tran.enable_encryption(false);

    if (!ep.node)
      return;

    auto const request = m_proto.encode_session_request(
        cipher::rabbit, m_session_tran.get_public_key());

    auto const n = ep.node->send(
        address, port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::set_remote_endpoint(span_cbyte req) noexcept {
    m_remote_port = m_proto.decode_session_response_port(req);
  }

  void server::set_remote_key(span_cbyte req) noexcept {
    m_session_tran.set_remote_key(m_proto.decode_public_key(req));
    m_session_tran.enable_encryption(true);
  }

  void server::send_session_response(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const response = m_proto.encode_session_response(
        m_session_node->get_port(), m_session_tran.get_public_key());

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_tran.encode(transfer::wrap(response)));

    m_session_tran.enable_encryption(true);
  }

  void server::send_request_token(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode(control::request_token);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::send_session_token(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode_session_token(
        m_session_token);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::send_client_enter(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode(control::client_enter);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::send_client_leave(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode(control::client_leave);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::send_server_quit(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode(control::server_quit);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::send_events(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode_server_clock(10);

    auto const n = node->send(
        m_remote_address, m_remote_port,
        m_session_tran.encode(transfer::wrap(request)));
  }

  void server::create_actor() noexcept {
    m_actor = m_exe.actor_create();
  }

  void server::remove_actor() noexcept {
    if (m_actor == id_undefined)
      return;

    m_exe.actor_remove(m_actor);
    m_actor = id_undefined;
  }

  void server::stop() noexcept {
    if (!is_connected())
      return;

    if (m_is_master)
      send_server_quit(m_session_node);
    else
      send_client_leave(m_session_node);

    m_is_connected = false;
  }
}
