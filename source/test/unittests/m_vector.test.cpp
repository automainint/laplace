/*  laplace/unittests/m_vector.test.cpp
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

#include "../../laplace/math/vector.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using math::dot, math::cross, math::vector;

  TEST(math, vector_comparison) {
    const auto a = vector<3, double>(1.0, 2.0, 3.0);
    const auto b = vector<3, double>(1.0, 2.0, 3.0);
    const auto c = vector<3, double>(2.0, 3.0, 1.0);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
  }

  TEST(math, vector_dot) {
    auto a = vector<3, double>(2.0, 2.0, 2.0);
    auto b = vector<3, double>(-2.0, 2.0, -1.0);

    const auto     d0 = dot(a, b);
    constexpr auto d1 = 2.0 * (-2) + 2. * 2. + (2. * -1.);

    EXPECT_NEAR(d0, d1, 1e-10);
  }

  TEST(math, vector_cross) {
    auto a = vector<3, double>(2.0, 2.0, 2.0);
    auto b = vector<3, double>(-2.0, 2.0, -1.0);

    const auto d0 = cross(a, b);

    const auto d1 = vector<3, double>(2.0 * (-1.) - 2. * 2.,
                                      2.0 * (-2.) - 2. * (-1.),
                                      2.0 * 2. - 2. * (-2.));

    EXPECT_TRUE(d0 == d1);
  }
}
