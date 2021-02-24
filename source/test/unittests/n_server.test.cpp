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
#include "../../laplace/engine/protocol/debug.h"
#include "../../laplace/engine/protocol/ids.h"
#include "../../laplace/network/host.h"
#include "../../laplace/network/remote.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using std::make_shared, network::host,
      network::remote, network::localhost,
      engine::basic_factory, engine::protocol::debug;

  namespace sets        = engine::object::sets;
  namespace ids         = engine::protocol::ids;
  namespace this_thread = std::this_thread;

  TEST(network, server_echo) {
    constexpr size_t test_count     = 3;
    constexpr size_t test_threshold = 1;

    size_t success = 0;

    for (size_t i = 0; i < test_count; i++) {
      auto my_host = make_shared<host>();
      auto client  = make_shared<remote>();

      my_host->set_factory(make_shared<basic_factory>());
      client->set_factory(make_shared<basic_factory>());

      uint16_t allowed_commands[] = { ids::debug,
                                      ids::client_enter };

      my_host->set_allowed_commands(allowed_commands);
      my_host->listen();

      client->connect(localhost, my_host->get_port());

      int64_t test_value = 12367;

      client->tick(0);
      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();

      client->emit<debug>(test_value);
      client->tick(0);

      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();
      client->tick(0);
      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();
      client->tick(0);

      int64_t echo_value = 0;

      if (auto w = client->get_world(); w) {
        if (auto root = w->get_root(); root) {
          root->adjust();

          echo_value = root->get(
              root->index_of(sets::debug_value));
        }
      }

      if (echo_value == test_value)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }

  TEST(network, server_encryption) {
    constexpr size_t test_count     = 3;
    constexpr size_t test_threshold = 1;

    size_t success = 0;

    for (size_t i = 0; i < test_count; i++) {
      auto my_host = make_shared<host>();
      auto client  = make_shared<remote>();

      my_host->set_factory(make_shared<basic_factory>());
      client->set_factory(make_shared<basic_factory>());

      uint16_t allowed_commands[] = { ids::debug,
                                      ids::client_enter,
                                      ids::public_key };

      my_host->set_allowed_commands(allowed_commands);
      my_host->listen();

      client->connect(localhost, my_host->get_port());

      int64_t test_value = 12367;

      client->tick(0);
      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();

      client->emit<debug>(test_value);
      client->tick(0);

      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();
      client->tick(0);
      this_thread::yield();
      my_host->tick(0);
      this_thread::yield();
      client->tick(0);

      int64_t echo_value = 0;

      if (auto w = client->get_world(); w) {
        if (auto root = w->get_root(); root) {
          root->adjust();

          echo_value = root->get(
              root->index_of(sets::debug_value));
        }
      }

      if (echo_value == test_value)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }
}
