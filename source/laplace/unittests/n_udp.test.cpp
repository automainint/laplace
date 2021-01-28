#include "../network/udp_node.h"
#include <gtest/gtest.h>
#include <thread>

using namespace laplace;
using namespace network;
using namespace std;

TEST(laplace_network, udp_message)
{
    udp_node a;
    udp_node b(any_port);

    vbyte msg = { 1, 2, 3 };

    a.send_to(localhost, b.get_port(), msg);

    this_thread::yield();

    auto seq = b.receive(msg.size(), async);

    EXPECT_EQ(msg, seq);
}

TEST(laplace_network, udp_echo)
{
    udp_node a;
    udp_node b(any_port);

    vbyte msg = { 1, 2, 3 };
    vbyte req = { 5 };
    vbyte seq;

    a.send_to(localhost, b.get_port(), req);

    this_thread::yield();

    if (b.receive(req.size(), async) == req)
    {
        b.send_to(b.get_remote_address(), b.get_remote_port(), msg);

        this_thread::yield();

        seq = a.receive(msg.size(), async);
    }

    EXPECT_EQ(msg, seq);
}
