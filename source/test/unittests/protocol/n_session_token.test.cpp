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

  TEST(network, session_token_client) {
    /*  Client sets up token received with session_token.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  request_token     ->
     *                    <-  session_token
     */

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    _setup_mock_protocol(bob);
    bob.enable_encryption(true);

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
    received      = _receive(alice, tran);
    auto bob_port = alice->get_remote_port();

    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_EQ(received[0].size(), 1);
      EXPECT_TRUE(_is(received[0], id_request_token));
    }

    auto token = network::random {}.generate_token();
    EXPECT_TRUE(_send(alice, bob_port, tran, id_session_token,
                      span_cbyte { token }));

    session.resume();

    auto bob_token = vbyte { bob.get_token().begin(),
                             bob.get_token().end() };
    EXPECT_EQ(bob_token, token);
  }

  TEST(network, session_token_host) {
    /*  Host finds saved token for received session_token and sets
     *  found actor id for remote peer.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  request_token     ->
     *                    <-  session_token
     *  (reset)
     *  session_request   ->
     *                    <-  session_response
     *  session_token     ->
     */
  }
}
