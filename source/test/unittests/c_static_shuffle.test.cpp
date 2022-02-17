/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/static_shuffle.h"
#include <gtest/gtest.h>
#include <random>

namespace laplace::test {
  using core::static_shuffle;

  TEST(core, static_shaffle_constexpr) {
    constexpr auto x = static_shuffle<0, 256>(1);

    EXPECT_NE(x[0], x[1]);
    EXPECT_NE(x[0], x[2]);
    EXPECT_NE(x[0], x[3]);
    EXPECT_NE(x[0], x[4]);
    EXPECT_NE(x[0], x[10]);
    EXPECT_NE(x[0], x[20]);
    EXPECT_NE(x[0], x[100]);
    EXPECT_NE(x[2], x[18]);

    EXPECT_GT(x.upper(2), x[1]);
    EXPECT_GT(x.upper(3), x[2]);
    EXPECT_GT(x.upper(4), x[3]);
    EXPECT_GT(x.upper(5), x[4]);
    EXPECT_GT(x.upper(11), x[10]);
    EXPECT_GT(x.upper(21), x[20]);
    EXPECT_GT(x.upper(101), x[100]);
  }

  TEST(core, static_shaffle_runtime) {
    auto dev = std::random_device {};

    for (unsigned k = 0; k < 20; k++) {
      auto x = static_shuffle<0, 256>(dev());

      for (unsigned n = 0; n < 100; n++) {
        EXPECT_TRUE(n == 1 || x[n] != x[1]);
        EXPECT_TRUE(n == 2 || x[n] != x[2]);
        EXPECT_TRUE(n == 3 || x[n] != x[3]);
        EXPECT_TRUE(n == 4 || x[n] != x[4]);
        EXPECT_TRUE(n == 10 || x[n] != x[10]);
        EXPECT_TRUE(n == 20 || x[n] != x[20]);
        EXPECT_TRUE(n == 100 || x[n] != x[100]);
        EXPECT_GT(x.upper(n + 1), x[n]);
      }
    }
  }
}
