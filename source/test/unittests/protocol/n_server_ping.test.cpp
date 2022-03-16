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

  TEST(network, server_ping_host) {
    /*  Peer answers to ping_request with ping_response.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *                    <-  ping_request
     *  ping_response     ->
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

    EXPECT_TRUE(_send(alice, alice->get_remote_port(), tran,
                      id_ping_request, sl::time { 123 }));

    session.resume();

    received = _receive(alice, tran);

    EXPECT_FALSE(received.empty());
    EXPECT_TRUE(_in(received, id_ping_response));

    for (auto &seq : received)
      if (_is(seq, id_ping_response)) {
        EXPECT_EQ(_get_ping_time(seq), 123);
      }
  }

  TEST(network, server_ping_client) {
    /*  Peer answers to ping_request with ping_response.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  ping_request      ->
     *                    <-  ping_response
     */

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock(alice);
    alice.enable_encryption(true);
    alice.set_tick_duration(15);

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

    EXPECT_TRUE(_send(bob, alice_port, tran, id_ping_request,
                      sl::time { 123 }));

    session.resume();

    received = _receive(bob, tran);

    EXPECT_FALSE(received.empty());
    EXPECT_TRUE(_in(received, id_ping_response));

    for (auto &seq : received)
      if (_is(seq, id_ping_response)) {
        EXPECT_EQ(_get_ping_time(seq), 123);
      }
  }
}
