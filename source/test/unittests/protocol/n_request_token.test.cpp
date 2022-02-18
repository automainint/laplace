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
      network::transfer, network::crypto::ecc_rabbit,
      network::any_port;

  TEST(network, request_token_session_token) {
    /*  Peer answers to request_token with session_token.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  request_token     ->
     *                    <-  session_token
     */

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock_protocol(alice);
    alice.enable_encryption(true);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob = io->open(2);

    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();

    EXPECT_TRUE(_send(bob, alice.get_port(), tran, id_session_request,
                      tran.get_public_key()));

    session.resume();

    auto alice_port = any_port;
    auto received   = _receive(bob, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_TRUE(_is(received[0], id_session_response));
      alice_port = _get_port(received[0]);
      tran.set_remote_key(_get_public_key(received[0]));
    }

    tran.enable_encryption(true);

    EXPECT_TRUE(_send(bob, alice_port, tran, id_request_token));

    session.resume();

    received = _receive(bob, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty())
      EXPECT_TRUE(_is(received[0], id_session_token));
  }
}
