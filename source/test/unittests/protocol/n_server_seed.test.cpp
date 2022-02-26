/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../../laplace/network/crypto/ecc_rabbit.h"
#include "../../../laplace/network/pipe.h"
#include "helpers.test.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::transfer, network::crypto::ecc_rabbit,
      network::any_port, network::id_undefined,
      network::index_undefined;

  TEST(network, server_seed_host) {
    /*  Host answers to client_enter with server_seed.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter      ->
     *                    <-  server_seed
     */

    auto actor_create_called = 0;

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock(alice, { .actor_create = [&]() -> sl::index {
                  actor_create_called++;
                  return 1;
                } });
    alice.enable_encryption(true);
    alice.set_random_seed(123);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob = io->open(2);

    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();

    EXPECT_TRUE(_send(bob, 1, tran, id_session_request,
                      tran.get_public_key()));

    session.resume();

    auto alice_port = any_port;

    auto received = _receive(bob, tran);
    EXPECT_EQ(received.size(), 1);
    if (!received.empty()) {
      EXPECT_TRUE(_is(received[0], id_session_response));
      alice_port = _get_port(received[0]);
      tran.set_remote_key(_get_public_key(received[0]));
    }

    tran.enable_encryption(true);

    EXPECT_TRUE(_send(bob, alice_port, tran, id_client_enter));

    session.resume();

    received = _receive(bob, tran);
    EXPECT_TRUE(_in(received, id_server_seed));
    for (auto &req : received) {
      if (!_is(req, id_server_seed))
        continue;
      EXPECT_NE(_index_of(req), index_undefined);
      EXPECT_EQ(_get_seed(req), 123);
    }
  }

  TEST(network, server_seed_client) {
    /*  Client sets random seed after he receives server_seed.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter      ->
     *                    <-  server_seed
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

    EXPECT_TRUE(_send(alice, alice->get_remote_port(), tran,
                      id_server_seed, sl::index { 0 },
                      sl::time { 123 }));

    session.resume();

    EXPECT_EQ(bob.get_random_seed(), 123);
  }
}
