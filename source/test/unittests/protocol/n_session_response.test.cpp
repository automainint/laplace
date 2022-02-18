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
#include "helpers.test.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  TEST(network, session_response_request_token) {
    /*  Peer answers to session_response with request_token if it has
     *  no token.
     *
     *    Bob                 Alice
     *  session_request   ->
     *                    <-  session_response
     *  request_token     ->
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;
    uint8_t const id_request_token    = 3;

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
      return serial::pack_to_bytes(id_session_request, key);
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
        return serial::pack_to_bytes(id_request_token);
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

    auto tran = transfer {};

    auto received = _receive(alice, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_EQ(
          serial::rd<decltype(id_session_request)>(received[0], 0),
          id_session_request);
      tran.setup_cipher<ecc_rabbit>();
      if (received[0].size() > sizeof id_session_request)
        tran.set_remote_key(
            { received[0].begin() + sizeof id_session_request,
              received[0].end() });
    }

    EXPECT_TRUE(_send(alice, 2, tran, id_session_response,
                      uint16_t { 3 }, tran.get_public_key()));

    alice = io->open(3);

    session.resume();

    tran.enable_encryption(true);
    received = _receive(alice, tran);

    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_EQ(received[0].size(), 1);
      EXPECT_EQ(
          serial::rd<decltype(id_request_token)>(received[0], 0),
          id_request_token);
    }

    EXPECT_EQ(encode_session_request_called, 1);
    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_session_response_port_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_called, 1);
  }

  TEST(network, session_response_session_token) {
    /*  Peer answers to session_response with session_token if it has
     *  a token.
     *
     *    Bob                 Alice
     *  session_request   ->
     *                    <-  session_response
     *  session_token     ->
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;
    uint8_t const id_session_token    = 3;

    int get_control_id_called               = 0;
    int decode_session_response_port_called = 0;
    int decode_public_key_called            = 0;
    int encode_session_request_called       = 0;
    int encode_session_token_called         = 0;

    auto proto                   = blank_protocol_interface();
    proto.encode_session_request = [&](cipher     cipher_id,
                                       span_cbyte key) -> vbyte {
      encode_session_request_called++;
      if (cipher_id != cipher::rabbit)
        return {};
      return serial::pack_to_bytes(id_session_request, key);
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
    proto.encode_session_token = [&](span_cbyte token) -> vbyte {
      encode_session_token_called++;
      return serial::pack_to_bytes(id_session_token, token);
    };

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    bob.setup_protocol(proto);
    bob.enable_encryption(true);

    auto token = network::random {}.generate_token();
    bob.set_token(token);

    auto session = bob.await_connect({ .io           = io,
                                       .host_address = "",
                                       .host_port    = 1,
                                       .client_port  = 2 });

    auto tran = transfer {};

    auto received = _receive(alice, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_EQ(
          serial::rd<decltype(id_session_request)>(received[0], 0),
          id_session_request);
      tran.setup_cipher<ecc_rabbit>();
      if (received[0].size() > sizeof id_session_request)
        tran.set_remote_key(
            { received[0].begin() + sizeof id_session_request,
              received[0].end() });
    }

    EXPECT_TRUE(_send(alice, 2, tran, id_session_response,
                      uint16_t { 3 }, tran.get_public_key()));

    alice = io->open(3);

    session.resume();

    tran.enable_encryption(true);

    received = _receive(alice, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_GT(received[0].size(), sizeof id_session_token);
      EXPECT_EQ(
          serial::rd<decltype(id_session_token)>(received[0], 0),
          id_session_token);
      if (received[0].size() > sizeof id_session_token) {
        auto received_token = vbyte { received[0].begin() +
                                          sizeof id_session_token,
                                      received[0].end() };
        EXPECT_EQ(token, received_token);
      }
    }

    EXPECT_EQ(encode_session_request_called, 1);
    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_session_response_port_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_session_token_called, 1);
  }
}
