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
      network::transfer, network::crypto::ecc_rabbit;

  TEST(network, session_response_client_enter_request_token) {
    /*  Peer answers to session_response with client_enter and
     *  request_token if he has no token.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter
     *  request_token     ->
     */

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    _setup_mock(bob);
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

    EXPECT_TRUE(_in(received, id_client_enter));
    EXPECT_TRUE(_in(received, id_request_token));
  }

  TEST(network, session_response_session_token) {
    /*  Peer answers to session_response with session_token if he has
     *  a token.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  session_token     ->
     */

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    _setup_mock(bob);
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
      EXPECT_TRUE(_is(received[0], id_session_request));
      tran.setup_cipher<ecc_rabbit>();
      tran.set_remote_key(_get_public_key(received[0]));
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
      EXPECT_TRUE(_is(received[0], id_session_token));
      auto span_token = _get_session_token(received[0]);
      auto bob_token = vbyte { span_token.begin(), span_token.end() };
      EXPECT_EQ(token, bob_token);
    }
  }
}
