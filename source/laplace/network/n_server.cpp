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

#include "../core/utils.h"
#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  using std::min, std::max, std::string, std::string_view,
      crypto::ecc_rabbit, std::span, std::unique_ptr,
      std::default_sentinel;

  server::~server() noexcept { }

  void server::setup_protocol(
      protocol_interface const &interface) noexcept {
    if (!check_protocol_interface(interface)) {
      error_("Invalid protocol interface.", __FUNCTION__);
      return;
    }

    m_proto = interface;
  }

  void server::setup_execution(
      execution_interface const &interface) noexcept {
    if (!check_execution_interface(interface)) {
      error_("Invalid event interface.", __FUNCTION__);
      return;
    }
  }

  void server::setup_log(log_interface const &interface) noexcept {
    if (!check_log_interface(interface)) {
      error_("Invalid log interface.", __FUNCTION__);
      return;
    }
  }

  void server::enable_encryption(bool is_enabled) noexcept { }

  void server::set_max_slot_count(sl::whole count) noexcept { }

  void server::set_token(span_cbyte token) noexcept {
    m_session_token.assign(token.begin(), token.end());
  }

  auto server::listen(span<endpoint_info const> endpoints) noexcept
      -> coroutine::task<> {
    m_is_connected = true;

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
    m_is_connected = true;

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

  auto server::get_port(sl::index io) const noexcept -> uint16_t {
    if (io < 0 || io >= m_endpoints.size()) {
      error_("Invalid IO index.", __FUNCTION__);
      return any_port;
    }

    if (!m_endpoints[io].node)
      return any_port;

    return m_endpoints[io].node->get_port();
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
    if (!m_proto.is_allowed(req, false))
      return;

    switch (m_proto.get_control_id(req)) {
      case control::session_request: session_open(ep, req); break;
      case control::session_response:
        set_remote_endpoint(req);
        set_remote_key(req);
        if (has_token())
          send_session_token(ep.node);
        else
          send_request_token(ep);
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

    auto const reqs = m_tran.decode({ buf, buf + received });

    for (auto &req : reqs) process_session(req);
  }

  void server::process_session(span_cbyte req) noexcept {
    if (!m_proto.is_allowed(req, true))
      return;

    switch (m_proto.get_control_id(req)) {
      case control::request_token:
        m_session_token = m_random.generate_token();
        send_session_token(m_session_node);
        break;
      default:;
    }
  }

  void server::session_open(endpoint const &ep,
                            span_cbyte      req) noexcept {
    m_session_node = ep.io->open(any_port);

    switch (m_proto.decode_cipher_id(req)) {
      case cipher::rabbit:
        m_tran.setup_cipher<ecc_rabbit>();
        m_tran.set_remote_key(m_proto.decode_public_key(req));
        break;
      default: return;
    }

    if (!ep.node)
      return;

    m_remote_address = ep.node->get_remote_address();
    m_remote_port    = ep.node->get_remote_port();

    auto const response = m_proto.encode_session_response(
        m_session_node->get_port(), m_tran.get_public_key());

    auto const n = ep.node->send(
        m_remote_address, m_remote_port,
        m_tran.encode(transfer::wrap(response)));

    m_tran.enable_encryption(true);
  }

  void server::send_session_request(endpoint const &ep,
                                    string_view     address,
                                    uint16_t        port) noexcept {
    m_tran.setup_cipher<ecc_rabbit>();

    if (!ep.node)
      return;

    auto const request = m_proto.encode_session_request(
        cipher::rabbit, m_tran.get_public_key());

    auto const n = ep.node->send(
        address, port, m_tran.encode(transfer::wrap(request)));
  }

  void server::set_remote_endpoint(span_cbyte req) noexcept {
    m_remote_port = m_proto.decode_session_response_port(req);
  }

  void server::set_remote_key(span_cbyte req) noexcept {
    m_tran.set_remote_key(m_proto.decode_public_key(req));
    m_tran.enable_encryption(true);
  }

  void server::send_request_token(endpoint const &ep) noexcept {
    if (!ep.node)
      return;

    auto const request = m_proto.encode(control::request_token);

    auto const n = ep.node->send(
        m_remote_address, m_remote_port,
        m_tran.encode(transfer::wrap(request)));
  }

  void server::send_session_token(ptr_node const &node) noexcept {
    if (!node)
      return;

    auto const request = m_proto.encode_session_token(
        m_session_token);

    auto const n = node->send(m_remote_address, m_remote_port,
                              m_tran.encode(transfer::wrap(request)));
  }
}
