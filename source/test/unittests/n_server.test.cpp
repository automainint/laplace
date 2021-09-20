/*  test/unittests/n_server.test.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/object/sets.h"
#include "../../laplace/engine/protocol/all.h"
#include "../../laplace/network/host.h"
#include "../../laplace/network/remote.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using std::this_thread::yield, std::make_shared, network::host,
      network::remote, network::localhost, engine::basic_factory,
      engine::protocol::debug;

  namespace sets = engine::object::sets;
  namespace ids  = engine::protocol::ids;

  TEST(network, server_echo) {
    constexpr sl::index test_count     = 3;
    constexpr sl::index test_threshold = 3;

    sl::index success = 0;

    for (sl::index i = 0; i < test_count; i++) {
      auto my_host = make_shared<host>();
      auto client  = make_shared<remote>();

      my_host->make_factory<basic_factory>();
      client->make_factory<basic_factory>();

      uint16_t allowed_commands[] = { ids::debug, ids::session_request,
                                      ids::client_enter };

      my_host->set_allowed_commands(allowed_commands);
      my_host->listen();

      client->set_encryption_enabled(false);
      client->connect(localhost, my_host->get_port());

      int64_t test_value = 12367;

      client->tick(0);
      yield();
      my_host->tick(0);
      yield();

      client->emit<debug>(test_value);
      client->tick(0);

      yield();
      my_host->tick(0);
      yield();
      client->tick(0);
      yield();
      my_host->tick(0);
      yield();
      client->tick(0);

      int64_t echo_value = 0;

      if (auto w = client->get_world(); w) {
        if (auto root = w->get_entity(w->get_root()); root) {
          root->adjust();

          echo_value = root->get(root->index_of(sets::debug_value));
        }
      }

      if (echo_value == test_value)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }

  TEST(network, server_encryption) {
    constexpr sl::index test_count     = 3;
    constexpr sl::index test_threshold = 3;

    sl::index success = 0;

    for (sl::index i = 0; i < test_count; i++) {
      auto my_host = make_shared<host>();
      auto client  = make_shared<remote>();

      my_host->make_factory<basic_factory>();
      client->make_factory<basic_factory>();

      uint16_t allowed_commands[] = { ids::debug, ids::session_request,
                                      ids::client_enter };

      my_host->set_allowed_commands(allowed_commands);
      my_host->listen();

      client->connect(localhost, my_host->get_port());

      constexpr int64_t test_value = 12367;

      client->tick(0);
      yield();
      my_host->tick(0);
      yield();

      client->emit<debug>(test_value);
      client->tick(0);

      yield();
      my_host->tick(0);
      yield();
      client->tick(0);
      yield();
      my_host->tick(0);
      yield();
      client->tick(0);

      int64_t echo_value = 0;

      if (auto w = client->get_world(); w) {
        if (auto root = w->get_entity(w->get_root()); root) {
          root->adjust();

          echo_value = root->get(root->index_of(sets::debug_value));
        }
      }

      if (echo_value == test_value)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }
}
