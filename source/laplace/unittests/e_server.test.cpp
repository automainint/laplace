#include "../engine/host.h"
#include "../engine/object/sets.h"
#include "../engine/protocol/debug.h"
#include "../engine/protocol/ids.h"
#include "../engine/remote.h"
#include <gtest/gtest.h>
#include <thread>

using namespace laplace;
using namespace engine;
using namespace object;
using namespace protocol;
using namespace std;

TEST(laplace_engine, server_echo) {
  size_t failure_count = 0;

  for (size_t i = 0; i < 10; i++) {
    constexpr auto address = "127.0.0.1";

    auto my_host = make_shared<host>();
    auto client  = make_shared<remote>();

    my_host->set_factory(make_shared<basic_factory>());
    client->set_factory(make_shared<basic_factory>());

    uint16_t allowed_commands[] = { ids::debug,
      ids::client_enter };

    my_host->set_allowed_commands(allowed_commands);
    my_host->listen();

    client->connect(address, my_host->get_port());

    int64_t test_value = 12367;

    client->tick(0);
    this_thread::yield();
    my_host->tick(0);
    this_thread::yield();

    client->queue(debug(test_value).encode());
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

        echo_value =
            root->get(root->index_of(sets::debug_value));
      }
    }

    if (echo_value != test_value)
      failure_count++;
  }

  EXPECT_LE(failure_count, 3);
}

TEST(laplace_engine, server_encryption) {
  size_t failure_count = 0;

  for (size_t i = 0; i < 10; i++) {
    constexpr auto address = "127.0.0.1";

    auto my_host = make_shared<host>();
    auto client  = make_shared<remote>();

    my_host->set_factory(make_shared<basic_factory>());
    client->set_factory(make_shared<basic_factory>());

    uint16_t allowed_commands[] = { ids::debug,
      ids::client_enter, ids::public_key };

    my_host->set_allowed_commands(allowed_commands);
    my_host->listen();

    client->connect(address, my_host->get_port());

    int64_t test_value = 12367;

    client->tick(0);
    this_thread::yield();
    my_host->tick(0);
    this_thread::yield();

    client->queue(debug(test_value).encode());
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

        echo_value =
            root->get(root->index_of(sets::debug_value));
      }
    }

    if (echo_value != test_value)
      failure_count++;
  }

  EXPECT_LE(failure_count, 3);
}
