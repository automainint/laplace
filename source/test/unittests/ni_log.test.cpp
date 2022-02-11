/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/interface/log.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::check_log_interface, network::blank_log_interface,
      network::log_interface;

  TEST(network, interface_log) {
    EXPECT_TRUE(check_log_interface(blank_log_interface()));
    EXPECT_FALSE(check_log_interface(log_interface {}));
  }
}
