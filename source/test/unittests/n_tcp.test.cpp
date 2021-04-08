/*  test/unittests/n_tcp.test.cpp
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

#include "../../laplace/network/tcp_joint.h"
#include "../../laplace/network/tcp_node.h"
#include <chrono>
#include <gtest/gtest.h>

namespace laplace::test {
  using network::tcp_node, network::tcp_joint, network::localhost,
      network::ref_tcp_node, network::ref_tcp_joint, network::async,
      std::chrono::steady_clock, std::chrono::duration_cast,
      std::chrono::seconds;

  TEST(network, tcp_server_to_client) {
    constexpr auto timeout_sec = 2;

    tcp_node  server;
    tcp_joint client(localhost, server.get_port());

    vbyte msg = { 1, 2, 3 };
    vbyte seq;

    server.on_accept([&msg](ref_tcp_node n) {
      std::ignore = n.send(msg);
      n.close_node();
    });

    client.on_connect([&seq](ref_tcp_joint j) {
      auto v = j.receive(3 - seq.size(), async);
      seq.insert(seq.end(), v.begin(), v.end());
      if (seq.size() == 3)
        j.close();
    });

    const auto t0 = steady_clock::now();

    while (!server.is_done() || !client.is_done()) {
      server.tick();
      client.tick();

      if (duration_cast<seconds>(steady_clock::now() - t0).count() >=
          timeout_sec)
        break;
    }

    EXPECT_EQ(seq, msg);
  }

  TEST(network, tcp_client_to_server) {
    constexpr auto timeout_sec = 2;

    tcp_node  server;
    tcp_joint client(localhost, server.get_port());

    vbyte msg = { 1, 2, 3 };
    vbyte seq;

    server.on_accept([&seq](ref_tcp_node n) {
      auto v = n.receive(3 - seq.size(), async);
      seq.insert(seq.end(), v.begin(), v.end());
      if (seq.size() == 3)
        n.close_node();
    });

    client.on_connect([&msg](ref_tcp_joint j) {
      std::ignore = j.send(msg);
      j.close();
    });

    const auto t0 = steady_clock::now();

    while (!server.is_done() || !client.is_done()) {
      server.tick();
      client.tick();

      if (duration_cast<seconds>(steady_clock::now() - t0).count() >=
          timeout_sec)
        break;
    }

    EXPECT_EQ(seq, msg);
  }
}
