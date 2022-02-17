/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/serial.h"
#include "../../laplace/network/crypto/ecc_rabbit.h"
#include "../../laplace/network/pipe.h"
#include "../../laplace/network/server.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  /*  If encryption is enabled, all the requests and responses should
   *  be encrypted except for session_request and session_response.
   */

  TEST(network, server_initial_state) {
    auto alice = server {};
    EXPECT_FALSE(alice.is_connected());
    EXPECT_FALSE(alice.is_quit());
  }

  TEST(network, session_request_session_response) {
    /*  Peer answers to session_request with session_response.
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;

    int is_allowed_called              = 0;
    int get_control_id_called          = 0;
    int decode_cipher_id_called        = 0;
    int decode_public_key_called       = 0;
    int encode_session_response_called = 0;

    auto proto       = blank_protocol_interface();
    proto.is_allowed = [&](span_cbyte seq, bool is_exclusive) {
      is_allowed_called++;
      return serial::rd<decltype(id_session_request)>(seq, 0) ==
             id_session_request;
    };
    proto.get_control_id = [&](span_cbyte seq) -> control {
      get_control_id_called++;
      if (serial::rd<decltype(id_session_request)>(seq, 0) ==
          id_session_request)
        return control::session_request;
      return control::undefined;
    };
    proto.decode_cipher_id = [&](span_cbyte seq) -> cipher {
      decode_cipher_id_called++;
      return cipher::rabbit;
    };
    proto.decode_public_key = [&](span_cbyte seq) -> span_cbyte {
      decode_public_key_called++;
      return { seq.begin() + sizeof id_session_request, seq.end() };
    };
    proto.encode_session_response = [&](uint16_t   port,
                                        span_cbyte key) -> vbyte {
      encode_session_response_called++;
      return serial::pack_to_bytes(id_session_response, port, key);
    };

    auto io = make_shared<pipe>();

    auto alice = server {};
    alice.setup_protocol(proto);
    alice.enable_encryption(true);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob = io->open(2);

    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();
    auto key = tran.get_public_key();

    auto const packed = tran.encode(transfer::wrap(
        serial::pack_to_bytes(id_session_request, key)));

    auto const sent = bob->send("", alice.get_port(), packed);
    EXPECT_EQ(sent, packed.size());

    session.resume();

    uint8_t    buf[1024] = {};
    auto const received  = bob->receive(buf);
    auto const plain     = tran.decode({ buf, buf + received });

    EXPECT_EQ(is_allowed_called, 1);
    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_cipher_id_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_session_response_called, 1);

    EXPECT_EQ(plain.size(), 1);
    if (!plain.empty())
      EXPECT_TRUE(serial::rd<decltype(id_session_response)>(
                      plain[0], 0) == id_session_response);
  }

  TEST(network, session_response_request_token) {
    /*  Peer answers to session_response with request_token if it has
     *  no token.
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;
    uint8_t const id_request_token    = 3;

    int is_allowed_called                   = 0;
    int get_control_id_called               = 0;
    int decode_session_response_port_called = 0;
    int decode_public_key_called            = 0;
    int encode_session_request_called       = 0;
    int encode_called                       = 0;

    auto proto                   = blank_protocol_interface();
    proto.encode_session_request = [&](cipher     cipher_id,
                                       span_cbyte key) -> vbyte {
      encode_session_request_called++;
      if (cipher_id != cipher::rabbit)
        return {};
      return serial::pack_to_bytes(uint8_t { 1 }, key);
    };
    proto.is_allowed = [&](span_cbyte seq, bool is_exclusive) {
      is_allowed_called++;
      return serial::rd<decltype(id_session_response)>(seq, 0) ==
             id_session_response;
    };
    proto.get_control_id = [&](span_cbyte seq) -> control {
      get_control_id_called++;
      if (serial::rd<decltype(id_session_response)>(seq, 0) ==
          id_session_response)
        return control::session_response;
      return control::undefined;
    };
    proto.decode_session_response_port =
        [&](span_cbyte seq) -> uint16_t {
      decode_session_response_port_called++;
      return serial::rd<uint16_t>(seq, 1);
    };
    proto.decode_public_key = [&](span_cbyte seq) -> span_cbyte {
      decode_public_key_called++;
      return { seq.begin() + 3, seq.end() };
    };
    proto.encode = [&](control control_id) -> vbyte {
      encode_called++;
      if (control_id == control::request_token)
        return serial::pack_to_bytes(uint8_t { 3 });
      return {};
    };

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    bob.setup_protocol(proto);
    bob.enable_encryption(true);

    auto session = bob.await_connect({ .io           = io,
                                       .host_address = "",
                                       .host_port    = 1,
                                       .client_port  = 2 });

    uint8_t buf[1024] = {};
    auto    received  = sl::whole {};
    auto    tran      = transfer {};

    received   = alice->receive(buf);
    auto plain = tran.decode({ buf, buf + received });
    EXPECT_EQ(plain.size(), 1);
    if (!plain.empty()) {
      EXPECT_EQ(serial::rd<decltype(id_session_request)>(plain[0], 0),
                id_session_request);
      tran.setup_cipher<ecc_rabbit>();
      if (plain[0].size() > sizeof id_session_request)
        tran.set_remote_key(
            { plain[0].begin() + sizeof id_session_request,
              plain[0].end() });
    }

    auto const packed = tran.encode(transfer::wrap(
        serial::pack_to_bytes(id_session_response, uint16_t { 3 },
                              tran.get_public_key())));

    auto const sent = alice->send("", 2, packed);
    EXPECT_EQ(sent, packed.size());

    alice = io->open(3);

    session.resume();

    received = alice->receive(buf);

    tran.enable_encryption(true);
    plain = tran.decode({ buf, buf + received });
    EXPECT_EQ(plain.size(), 1);
    if (!plain.empty()) {
      EXPECT_EQ(plain[0].size(), 1);
      EXPECT_EQ(serial::rd<decltype(id_request_token)>(plain[0], 0),
                id_request_token);
    }

    EXPECT_EQ(encode_session_request_called, 1);
    EXPECT_EQ(is_allowed_called, 1);
    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_session_response_port_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_called, 1);
  }

  TEST(network, session_response_session_token) {
    /*  Peer answers to session_response with session_token if it has
     *  a token.
     */
  }

  TEST(network, server_session_token) { }

  TEST(network, server_request_events) { }

  TEST(network, server_ping) { }

  TEST(network, server_heartbeat) { }

  TEST(network, server_reserve) { }

  TEST(network, server_init) { }

  TEST(network, server_quit) { }

  TEST(network, server_clock) { }

  TEST(network, server_seed) { }

  TEST(network, server_action) { }

  TEST(network, server_pause) { }

  TEST(network, server_loading) { }

  TEST(network, client_enter) { }

  TEST(network, client_leave) { }

  TEST(network, client_ready) { }

  TEST(network, slot_create) { }

  TEST(network, slot_remove) { }
}
