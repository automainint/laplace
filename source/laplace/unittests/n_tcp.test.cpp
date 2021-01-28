#include "../network/node.h"
#include "../network/joint.h"
#include <gtest/gtest.h>

using namespace laplace;
using namespace network;
using namespace std;

TEST(laplace_network, tcp_server_to_client)
{
    node server;
    joint client(localhost, server.get_port());

    vbyte msg = { 1, 2, 3 };
    vbyte seq;

    server.on_accept([&msg](ref_node n) {
        n.send(msg);
        n.close_node();
    });

    client.on_connect([&seq](ref_joint j) {
        auto v = j.receive(3 - seq.size(), async);
        seq.insert(seq.end(), v.begin(), v.end());
        if (seq.size() == 3) j.close();
    });

    while (!server.is_done() || !client.is_done())
    {
        server.tick();
        client.tick();
    }

    EXPECT_EQ(seq, msg);
}

TEST(laplace_network, tcp_client_to_server)
{
    node server;
    joint client(localhost, server.get_port());

    vbyte msg = { 1, 2, 3 };
    vbyte seq;

    server.on_accept([&seq](ref_node n) {
        auto v = n.receive(3 - seq.size(), async);
        seq.insert(seq.end(), v.begin(), v.end());
        if (seq.size() == 3) n.close_node();
    });

    client.on_connect([&msg](ref_joint j) {
        j.send(msg);
        j.close();
    });

    while (!server.is_done() || !client.is_done())
    {
        server.tick();
        client.tick();
    }

    EXPECT_EQ(seq, msg);
}
