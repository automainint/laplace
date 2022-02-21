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
      network::any_port, network::id_undefined;

  TEST(network, server_quit_host) {
    /*  Host sends server_quit before exit.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *                    <-  server_quit
     *                        (exit)
     */

    auto removed_actor = id_undefined;

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock(alice, { .actor_remove = [&](sl::index actor) {
                  removed_actor = actor;
                } });
    alice.enable_encryption(true);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob = io->open(2);

    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();

    EXPECT_TRUE(_send(bob, 1, tran, id_session_request,
                      tran.get_public_key()));

    session.resume();

    auto received = _receive(bob, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_TRUE(_is(received[0], id_session_response));
      tran.set_remote_key(_get_public_key(received[0]));
    }

    tran.enable_encryption(true);

    /*  Stop the server.
     */
    alice = server {};

    received = _receive(bob, tran);
    EXPECT_TRUE(_in(received, id_server_quit));
  }
}
