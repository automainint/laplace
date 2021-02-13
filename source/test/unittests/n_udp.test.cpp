/*  laplace/unittests/n_udp.test.cpp
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

#include "../../laplace/network/udp_node.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using network::udp_node, network::any_port,
      network::localhost, network::async;

  namespace this_thread = std::this_thread;

  TEST(network, udp_message) {
    constexpr size_t test_count     = 3;
    constexpr size_t test_threshold = 1;

    size_t success = 0;

    for (size_t i = 0; i < test_count; i++) {
      udp_node a;
      udp_node b(any_port);

      vbyte msg = { 1, 2, 3 };

      a.send_to(localhost, b.get_port(), msg);

      this_thread::yield();

      auto seq = b.receive(msg.size(), async);

      if (msg == seq)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }

  TEST(network, udp_echo) {
    constexpr size_t test_count     = 3;
    constexpr size_t test_threshold = 1;

    size_t success = 0;

    for (size_t i = 0; i < test_count; i++) {
      udp_node a;
      udp_node b(any_port);

      vbyte msg = { 1, 2, 3 };
      vbyte req = { 5 };
      vbyte seq;

      a.send_to(localhost, b.get_port(), req);

      this_thread::yield();

      if (b.receive(req.size(), async) == req) {
        b.send_to(
            b.get_remote_address(), b.get_remote_port(), msg);

        this_thread::yield();

        seq = a.receive(msg.size(), async);
      }

      if (msg == seq)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }
}
