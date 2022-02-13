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
      crypto::ecc_rabbit, std::span, std::unique_ptr;

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

  void server::listen(span<endpoint_info const> endpoints) noexcept {
    for (auto &ep : endpoints)
      m_endpoints.emplace_back<endpoint>(
          { .io = ep.io, .node = ep.io->open(ep.port) });
  }

  void server::connect(connect_info info) noexcept { }

  void server::queue(span_cbyte seq) noexcept { }

  void server::tick(sl::time delta_msec) noexcept {
    read_endpoints();
  }

  auto server::get_port(sl::index io) const noexcept -> uint16_t {
    if (io < 0 || io >= m_endpoints.size()) {
      error_("Invalid IO index.", __FUNCTION__);
      return any_port;
    }

    if (!m_endpoints[io].node)
      return any_port;

    return m_endpoints[io].node->get_port();
  }

  auto server::is_connected() const noexcept -> bool {
    return false;
  }

  auto server::is_quit() const noexcept -> bool {
    return false;
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
      case control::session_request: session_open(ep, req); return;
      default:;
    }
  }

  void server::session_open(endpoint const &ep,
                            span_cbyte      req) noexcept {
    auto session = ep.io->open(any_port);
    auto tran    = transfer {};

    switch (m_proto.decode_cipher_id(req)) {
      case cipher::rabbit:
        tran.setup_cipher<ecc_rabbit>();
        tran.set_remote_key(m_proto.decode_public_key(req));
        break;
      default: return;
    }

    if (!ep.node)
      return;

    auto const address  = ep.node->get_remote_address();
    auto const port     = ep.node->get_remote_port();
    auto const response = m_proto.encode_session_response(
        session->get_port(), tran.get_public_key());

    auto const n = ep.node->send(address, port, response);
  }
}
