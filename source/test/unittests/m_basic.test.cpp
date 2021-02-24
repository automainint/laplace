/*  test/unittests/m_basic.test.cpp
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

#include "../../laplace/math/basic.h"
#include "../../laplace/math/shortnames.h"
#include "../../laplace/math/vector.h"
#include <gtest/gtest.h>
#include <random>

namespace laplace::test {
  using math::equals, math::get_zero, math::get_unit,
      math::length, math::shortnames::vec2;

  TEST(math, basic_equals) {
    EXPECT_TRUE(equals(0.f, 0.f));

    EXPECT_TRUE(equals(   //
        get_zero<vec2>(), //
        get_zero<vec2>()  //
        ));

    EXPECT_FALSE(equals(  //
        get_zero<vec2>(), //
        get_unit<vec2>()  //
        ));
  }

  TEST(math, basic_length) {
    auto a = vec2 { 3., 4. };
    auto l = length(a);

    EXPECT_DOUBLE_EQ(l, 5.);
  }
}
