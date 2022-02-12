/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/random.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::random;

  TEST(network, random_seed) {
    int ok = 0;

    for (int i = 0; i < 3; i++)
      if (random {}.get_seed() != random {}.get_seed())
        ok++;

    EXPECT_GE(ok, 2);
  }

  TEST(network, random_token) {
    EXPECT_GE(random {}.generate_token().size(), 128);
    EXPECT_NE(random {}.generate_token(), random {}.generate_token());
  }
}
