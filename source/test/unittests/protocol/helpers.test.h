/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_PROTOCOL_HELPERS_TEST_H
#define LAPLACE_NETWORK_PROTOCOL_HELPERS_TEST_H

#include "../../../laplace/core/serial.h"
#include "../../../laplace/network/interface/io.h"
#include "../../../laplace/network/transfer.h"

namespace laplace::test {
  /*  Mock networking protocol.
   */

  static constexpr uint8_t id_session_request  = 1;
  static constexpr uint8_t id_session_response = 2;
  static constexpr uint8_t id_session_token    = 3;
  static constexpr uint8_t id_request_token    = 4;
  static constexpr uint8_t id_client_enter     = 5;
  static constexpr uint8_t id_client_leave     = 6;
  static constexpr uint8_t id_server_quit      = 7;

  [[nodiscard]] inline auto _is(span_cbyte seq, uint8_t id) noexcept
      -> bool {
    return serial::rd<uint8_t>(seq, 0) == id;
  }

  [[nodiscard]] inline auto _in(std::span<vbyte> seqs,
                                uint8_t id) noexcept -> bool {
    for (auto &seq : seqs)
      if (_is(seq, id))
        return true;
    return false;
  }

  [[nodiscard]] inline auto _get_port(span_cbyte seq) noexcept
      -> uint16_t {
    if (_is(seq, id_session_response))
      return serial::rd<uint16_t>(seq, sizeof id_session_response);
    return network::any_port;
  }

  [[nodiscard]] inline auto _get_public_key(span_cbyte seq) noexcept
      -> span_cbyte {
    if (_is(seq, id_session_request) &&
        seq.size() > sizeof id_session_request)
      return { seq.begin() + sizeof id_session_request, seq.end() };
    if (_is(seq, id_session_response) &&
        seq.size() > (sizeof id_session_request) + sizeof(uint16_t))
      return { seq.begin() + (sizeof id_session_request) +
                   sizeof(uint16_t),
               seq.end() };
    return {};
  }

  [[nodiscard]] inline auto _get_session_token(
      span_cbyte seq) noexcept -> span_cbyte {
    if (_is(seq, id_session_token) &&
        seq.size() > sizeof id_session_token)
      return { seq.begin() + sizeof id_session_token, seq.end() };
    return {};
  }

  struct _mock_callbacks {
    network::fn_actor_create actor_create =
        [n = 0]() mutable -> sl::index { return n++; };
    network::fn_actor_remove actor_remove = [](sl::index) {};
  };

  inline void _setup_mock(network::server &peer,
                          _mock_callbacks  callbacks = {}) noexcept {
    auto proto       = network::blank_protocol_interface();
    proto.is_allowed = [&](span_cbyte seq) {
      return _is(seq, id_request_token) ||
             _is(seq, id_session_token) ||
             _is(seq, id_client_enter) || _is(seq, id_client_leave) ||
             _is(seq, id_server_quit);
    };
    proto.get_control_id = [&](span_cbyte seq) -> network::control {
      if (_is(seq, id_session_request))
        return network::control::session_request;
      if (_is(seq, id_session_response))
        return network::control::session_response;
      if (_is(seq, id_session_token))
        return network::control::session_token;
      if (_is(seq, id_request_token))
        return network::control::request_token;
      if (_is(seq, id_client_enter))
        return network::control::client_enter;
      if (_is(seq, id_client_leave))
        return network::control::client_leave;
      if (_is(seq, id_server_quit))
        return network::control::server_quit;
      return network::control::undefined;
    };
    proto.decode_cipher_id = [&](span_cbyte seq) -> network::cipher {
      return network::cipher::rabbit;
    };
    proto.decode_session_response_port =
        [&](span_cbyte seq) -> uint16_t { return _get_port(seq); };
    proto.decode_public_key = [&](span_cbyte seq) -> span_cbyte {
      return _get_public_key(seq);
    };
    proto.decode_session_token = [&](span_cbyte seq) -> span_cbyte {
      return _get_session_token(seq);
    };
    proto.encode_session_request = [&](network::cipher cipher_id,
                                       span_cbyte      key) -> vbyte {
      if (cipher_id != network::cipher::rabbit)
        return {};
      return serial::pack_to_bytes(id_session_request, key);
    };
    proto.encode_session_response = [&](uint16_t   port,
                                        span_cbyte key) -> vbyte {
      return serial::pack_to_bytes(id_session_response, port, key);
    };
    proto.encode_session_token = [&](span_cbyte token) -> vbyte {
      return serial::pack_to_bytes(id_session_token, token);
    };
    proto.encode = [&](network::control control_id) -> vbyte {
      switch (control_id) {
        case network::control::request_token:
          return serial::pack_to_bytes(id_request_token);
        case network::control::client_enter:
          return serial::pack_to_bytes(id_client_enter);
        case network::control::client_leave:
          return serial::pack_to_bytes(id_client_leave);
        case network::control::server_quit:
          return serial::pack_to_bytes(id_server_quit);
        default:;
      }
      return {};
    };
    peer.setup_protocol(proto);

    auto exe         = network::blank_execution_interface();
    exe.actor_create = callbacks.actor_create;
    exe.actor_remove = callbacks.actor_remove;
    peer.setup_execution(exe);
  }

  [[nodiscard]] inline auto _send_multiple(
      network::ptr_node &node, uint16_t port, network::transfer &tran,
      sl::vector<vbyte> const &requests) noexcept -> bool {
    auto packed = tran.encode(network::transfer::wrap(requests));
    return node->send("", port, packed) == packed.size();
  }

  [[nodiscard]] inline auto _send(network::ptr_node &node,
                                  uint16_t           port,
                                  network::transfer &tran,
                                  auto... args) noexcept -> bool {
    auto packed = tran.encode(
        network::transfer::wrap(serial::pack_to_bytes(args...)));
    return node->send("", port, packed) == packed.size();
  }

  [[nodiscard]] inline auto _receive(network::ptr_node &node,
                                     network::transfer &tran) noexcept
      -> sl::vector<vbyte> {
    static uint8_t buf[1024] = {};
    return tran.decode({ buf, buf + node->receive(buf) });
  }
}

#endif
