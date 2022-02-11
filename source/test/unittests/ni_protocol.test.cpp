/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/interface/protocol.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::check_protocol_interface,
      network::blank_protocol_interface, network::protocol_interface;

  TEST(network, interface_protocol) {
    EXPECT_TRUE(check_protocol_interface(blank_protocol_interface()));
    EXPECT_FALSE(check_protocol_interface(protocol_interface {}));
  }
}
