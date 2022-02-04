/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/udp_ipv4_node.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using network::udp_ipv4_node, network::any_port, network::localhost,
      network::async;

  namespace this_thread = std::this_thread;

  TEST(network, udp_ipv4_node_message) {
    constexpr sl::index test_count     = 3;
    constexpr sl::index test_threshold = 1;

    sl::index success = 0;

    for (sl::index i = 0; i < test_count; i++) {
      auto a = udp_ipv4_node {};
      auto b = udp_ipv4_node(any_port);

      auto msg = vbyte { 1, 2, 3 };

      if (a.send(localhost, b.get_port(), msg) != msg.size())
        continue;

      this_thread::yield();

      auto seq = vbyte(msg.size());
      seq.resize(b.receive(seq, async));

      if (msg == seq)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }

  TEST(network, udp_ipv4_node_echo) {
    constexpr sl::index test_count     = 3;
    constexpr sl::index test_threshold = 1;

    sl::index success = 0;

    for (sl::index i = 0; i < test_count; i++) {
      auto a = udp_ipv4_node {};
      auto b = udp_ipv4_node(any_port);

      auto msg = vbyte { 1, 2, 3 };
      auto req = vbyte { 5 };

      if (a.send(localhost, b.get_port(), req) != req.size())
        continue;

      this_thread::yield();

      auto seq1 = vbyte(req.size());
      seq1.resize(b.receive(seq1, async));

      if (seq1 != req)
        continue;

      if (b.send(b.get_remote_address(), b.get_remote_port(), msg) !=
          msg.size())
        continue;

      this_thread::yield();

      auto seq2 = vbyte(msg.size());
      seq2.resize(a.receive(seq2, async));

      if (msg == seq2)
        success++;
    }

    EXPECT_GE(success, test_threshold);
  }
}
