/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../../laplace/core/serial.h"
#include "../../../laplace/network/crypto/ecc_rabbit.h"
#include "../../../laplace/network/pipe.h"
#include "../../../laplace/network/server.h"
#include <gtest/gtest.h>

namespace laplace::network {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  TEST(network, request_token_session_token) {
    /*  Peer answers to request_token with session_token.
     *
     *    Bob                 Alice
     *  session_request   ->
     *                    <-  session_response
     *  request_token     ->
     *                    <-  session_token
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;
    uint8_t const id_session_token    = 3;
    uint8_t const id_request_token    = 4;

    int is_allowed_called              = 0;
    int get_control_id_called          = 0;
    int decode_cipher_id_called        = 0;
    int decode_public_key_called       = 0;
    int encode_session_response_called = 0;
    int encode_session_token_called    = 0;

    auto proto       = blank_protocol_interface();
    proto.is_allowed = [&](span_cbyte seq, bool is_exclusive) {
      is_allowed_called++;
      return serial::rd<decltype(id_session_request)>(seq, 0) ==
                 id_session_request ||
             serial::rd<decltype(id_request_token)>(seq, 0) ==
                 id_request_token;
    };
    proto.get_control_id = [&](span_cbyte seq) -> control {
      get_control_id_called++;
      if (serial::rd<decltype(id_session_request)>(seq, 0) ==
          id_session_request)
        return control::session_request;
      if (serial::rd<decltype(id_request_token)>(seq, 0) ==
          id_request_token)
        return control::request_token;
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
    proto.encode_session_token = [&](span_cbyte token) -> vbyte {
      encode_session_token_called++;
      return serial::pack_to_bytes(id_session_token, token);
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

    auto packed = tran.encode(transfer::wrap(
        serial::pack_to_bytes(id_session_request, key)));
    auto sent   = bob->send("", alice.get_port(), packed);
    EXPECT_EQ(sent, packed.size());

    session.resume();

    uint8_t buf[1024] = {};
    auto    received  = bob->receive(buf);
    auto    plain     = tran.decode({ buf, buf + received });

    auto alice_port = any_port;

    EXPECT_EQ(plain.size(), 1);
    if (!plain.empty()) {
      EXPECT_TRUE(serial::rd<decltype(id_session_response)>(
                      plain[0], 0) == id_session_response);
      alice_port = serial::rd<decltype(alice_port)>(
          plain[0], sizeof id_session_response);
      if (plain[0].size() >
          sizeof id_session_response + sizeof alice_port)
        tran.set_remote_key(span_cbyte {
            plain[0].begin() + sizeof id_session_response +
                sizeof alice_port,
            plain[0].end() });
    }

    tran.enable_encryption(true);

    packed = tran.encode(
        transfer::wrap(serial::pack_to_bytes(id_request_token)));
    sent = bob->send("", alice_port, packed);
    EXPECT_EQ(sent, packed.size());

    session.resume();

    received = bob->receive(buf);
    plain    = tran.decode({ buf, buf + received });
    EXPECT_EQ(plain.size(), 1);
    if (!plain.empty())
      EXPECT_GT(plain[0].size(), sizeof id_session_token);

    EXPECT_EQ(is_allowed_called, 2);
    EXPECT_EQ(get_control_id_called, 2);
    EXPECT_EQ(decode_cipher_id_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_session_response_called, 1);
    EXPECT_EQ(encode_session_token_called, 1);
  }
}
