/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/interface/execution.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::check_execution_interface,
      network::blank_execution_interface,
      network::execution_interface;

  TEST(network, interface_execution) {
    EXPECT_TRUE(
        check_execution_interface(blank_execution_interface()));
    EXPECT_FALSE(check_execution_interface(execution_interface {}));
  }
}
