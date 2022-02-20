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

  TEST(network, session_request_session_response) {
    /*  Peer answers to session_request with session_response.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     */

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock(alice);
    alice.enable_encryption(true);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob  = io->open(2);
    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();

    EXPECT_TRUE(_send(bob, alice.get_port(), tran, id_session_request,
                      tran.get_public_key()));

    session.resume();

    auto const received = _receive(bob, tran);

    EXPECT_EQ(received.size(), 1);
    if (!received.empty())
      EXPECT_TRUE(_is(received[0], id_session_response));
  }
}
