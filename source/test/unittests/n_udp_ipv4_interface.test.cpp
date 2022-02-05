/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/udp_ipv4_interface.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using std::make_unique, network::udp_ipv4_interface;

  TEST(network, udp_ipv4_interface_open) {
    auto p = make_unique<udp_ipv4_interface>();
    EXPECT_TRUE(p->open(0));
  }
}
