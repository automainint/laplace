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
      network::any_port, network::id_undefined,
      network::time_undefined;

  TEST(network, slot_create_host_1) {
    /*  Host sends slot_create when he creates an actor.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter      ->
     *                    <-  slot_create (local)
     */

    auto io = make_shared<pipe>();

    auto alice = server {};
    _setup_mock(alice);
    alice.enable_encryption(true);

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

    auto bob_actor = id_undefined;

    if (auto actors = alice.get_actors(); !actors.empty())
      bob_actor = actors[0];

    received = _receive(bob, tran);
    EXPECT_TRUE(_in(received, id_slot_create));

    for (auto &seq : received)
      if (_is(seq, id_slot_create)) {
        EXPECT_EQ(_actor_of(seq), bob_actor);
        EXPECT_TRUE(_slot_is_local(seq));
      }
  }

  TEST(network, slot_create_client) {
    /*  Peer creates a new actor when receiving slot_create by
     *  performing slot_create event.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *                        slot_create (local)
     *                    <-  slot_create (remote)
     */

    sl::index const actor_bob   = 1;
    sl::index const actor_other = 2;

    int  actor_bob_created   = 0;
    int  actor_other_created = 0;
    bool actor_bob_local     = false;
    bool actor_other_local   = false;

    auto do_perform = [&](span_cbyte req) {
      if (!_is(req, id_slot_create))
        return;

      auto const actor = _actor_of(req);

      switch (actor) {
        case actor_bob:
          actor_bob_created++;
          actor_bob_local = _slot_is_local(req);
          break;
        case actor_other:
          actor_other_created++;
          actor_other_local = _slot_is_local(req);
          break;
      }
    };

    auto io = make_shared<pipe>();

    auto alice = io->open(1);

    auto bob = server {};
    _setup_mock(bob, { .do_perform = do_perform });
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
    std::ignore = _receive(alice, tran);

    EXPECT_TRUE(_send(alice, alice->get_remote_port(), tran,
                      id_slot_create, sl::index { 0 }, time_undefined,
                      actor_bob, uint8_t { 1 }));
    EXPECT_TRUE(_send(alice, alice->get_remote_port(), tran,
                      id_slot_create, sl::index { 1 }, time_undefined,
                      actor_other, uint8_t { 0 }));

    session.resume();

    EXPECT_EQ(actor_bob_created, 1);
    EXPECT_EQ(actor_other_created, 1);
    EXPECT_TRUE(actor_bob_local);
    EXPECT_FALSE(actor_other_local);
  }

  TEST(network, slot_create_host_2) {
    /*  Host sends slot_create when he creates an actor. 2 actors.
     *
     *    Bob                   Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter      ->
     *                    <-  slot_create (local)
     *                    <-  slot_create (remote)
     *
     *    Clyde                 Alice
     *  session_request   ->
     *                    <-  session_response
     *  client_enter      ->
     *                    <-  slot_create (remote)
     *                    <-  slot_create (local)
     */
  }
}
