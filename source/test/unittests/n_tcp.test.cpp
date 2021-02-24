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
#include <gtest/gtest.h>

namespace laplace::test {
  using network::tcp_node, network::tcp_joint,
      network::localhost, network::ref_tcp_node,
      network::ref_tcp_joint, network::async;

  TEST(network, tcp_server_to_client) {
    tcp_node  server;
    tcp_joint client(localhost, server.get_port());

    vbyte msg = { 1, 2, 3 };
    vbyte seq;

    server.on_accept([&msg](ref_tcp_node n) {
      static_cast<void>(n.send(msg));
      n.close_node();
    });

    client.on_connect([&seq](ref_tcp_joint j) {
      auto v = j.receive(3 - seq.size(), async);
      seq.insert(seq.end(), v.begin(), v.end());
      if (seq.size() == 3)
        j.close();
    });

    while (!server.is_done() || !client.is_done()) {
      server.tick();
      client.tick();
    }

    EXPECT_EQ(seq, msg);
  }

  TEST(network, tcp_client_to_server) {
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
      static_cast<void>(j.send(msg));
      j.close();
    });

    while (!server.is_done() || !client.is_done()) {
      server.tick();
      client.tick();
    }

    EXPECT_EQ(seq, msg);
  }
}
