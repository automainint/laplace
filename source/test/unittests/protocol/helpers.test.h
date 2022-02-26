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
#include "../../../laplace/network/interface/protocol.h"
#include "../../../laplace/network/server.h"

namespace laplace::test {
  /*  Mock networking protocol.
   */

  using id_type = uint8_t;

  static constexpr id_type id_session_request  = 1;
  static constexpr id_type id_session_response = 2;
  static constexpr id_type id_session_token    = 3;
  static constexpr id_type id_request_token    = 4;

  static constexpr id_type id_server_clock = 5;
  static constexpr id_type id_server_seed  = 6;
  static constexpr id_type id_server_init  = 7;
  static constexpr id_type id_server_quit  = 8;
  static constexpr id_type id_client_enter = 9;
  static constexpr id_type id_client_leave = 10;

  static constexpr sl::index n_id    = 0;
  static constexpr sl::index n_index = sizeof(id_type);
  static constexpr sl::index n_time  = n_index + sizeof(sl::index);

  static constexpr sl::index n_clock_time = n_time;
  static constexpr sl::index n_seed       = n_time;

  [[nodiscard]] inline auto _id_of(span_cbyte seq) -> id_type {
    return serial::rd<id_type>(seq, 0);
  }

  [[nodiscard]] inline auto _index_of(span_cbyte seq) -> sl::index {
    if (_id_of(seq) < id_server_clock)
      return network::index_undefined;
    return serial::rd<sl::index>(seq, n_index);
  }

  [[nodiscard]] inline auto _is(span_cbyte seq, id_type id) noexcept
      -> bool {
    return serial::rd<id_type>(seq, 0) == id;
  }

  [[nodiscard]] inline auto _in(std::span<vbyte> seqs,
                                id_type id) noexcept -> bool {
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

  [[nodiscard]] inline auto _get_clock_time(span_cbyte seq) noexcept
      -> sl::time {
    return serial::rd<sl::time>(seq, n_clock_time);
  }

  [[nodiscard]] inline auto _get_seed(span_cbyte seq) noexcept
      -> uint64_t {
    return serial::rd<uint64_t>(seq, n_seed);
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
             _is(seq, id_server_clock) || _is(seq, id_server_seed) ||
             _is(seq, id_server_init) || _is(seq, id_server_quit);
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
      if (_is(seq, id_server_clock))
        return network::control::server_clock;
      if (_is(seq, id_server_seed))
        return network::control::server_seed;
      if (_is(seq, id_server_init))
        return network::control::server_init;
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
    proto.decode_server_clock = [&](span_cbyte seq) -> sl::time {
      return serial::rd<uint64_t>(seq, n_clock_time);
    };
    proto.decode_server_seed = [&](span_cbyte seq) -> uint64_t {
      return serial::rd<uint64_t>(seq, n_seed);
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
    proto.encode_server_clock = [&](sl::time tick_duration) -> vbyte {
      return serial::pack_to_bytes(id_server_clock, sl::index {},
                                   tick_duration);
    };
    proto.encode_server_seed = [&](uint64_t seed) -> vbyte {
      return serial::pack_to_bytes(id_server_seed, sl::index {},
                                   seed);
    };
    proto.encode = [&](network::control control_id) -> vbyte {
      switch (control_id) {
        case network::control::request_token:
          return serial::pack_to_bytes(id_request_token);
        case network::control::client_enter:
          return serial::pack_to_bytes(id_client_enter, sl::index {});
        case network::control::client_leave:
          return serial::pack_to_bytes(id_client_leave, sl::index {});
        case network::control::server_init:
          return serial::pack_to_bytes(id_server_init, sl::index {});
        case network::control::server_quit:
          return serial::pack_to_bytes(id_server_quit, sl::index {});
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
    static id_type buf[1024] = {};
    return tran.decode({ buf, buf + node->receive(buf) });
  }
}

#endif
