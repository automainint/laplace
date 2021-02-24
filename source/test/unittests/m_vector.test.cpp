/*  test/unittests/m_vector.test.cpp
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

#include "../../laplace/math/shortnames.h"
#include "../../laplace/math/vector.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using math::equals, math::add, math::sub, math::mul,
      math::div, math::mul_by_elem, math::div_by_elem, math::dot,
      math::cross, math::square_length, math::shortnames::vec3;

  TEST(math, vector_arithmetic_constexpr) {
    constexpr auto a = vec3 { 1., 2., 3. };
    constexpr auto b = vec3 { 4., 5., 6. };

    constexpr auto add1 = add(a, b);
    constexpr auto sub1 = sub(a, b);
    constexpr auto mul1 = mul(a, 2.);
    constexpr auto mul2 = mul(2., a);
    constexpr auto mul3 = mul_by_elem(a, b);
    constexpr auto div1 = div(a, 2.);
    constexpr auto div2 = div(6., a);
    constexpr auto div3 = div_by_elem(b, a);

    EXPECT_TRUE(equals(     //
        add1,               //
        vec3 { 5., 7., 9. } //
        ));

    EXPECT_TRUE(equals(        //
        sub1,                  //
        vec3 { -3., -3., -3. } //
        ));

    EXPECT_TRUE(equals(     //
        mul1,               //
        vec3 { 2., 4., 6. } //
        ));

    EXPECT_TRUE(equals(     //
        mul2,               //
        vec3 { 2., 4., 6. } //
        ));

    EXPECT_TRUE(equals(       //
        mul3,                 //
        vec3 { 4., 10., 18. } //
        ));

    EXPECT_TRUE(equals(      //
        div1,                //
        vec3 { .5, 1., 1.5 } //
        ));

    EXPECT_TRUE(equals(     //
        div2,               //
        vec3 { 6., 3., 2. } //
        ));

    EXPECT_TRUE(equals(      //
        div3,                //
        vec3 { 4., 2.5, 2. } //
        ));
  }

  TEST(math, vector_arithmetic) {
    auto a = vec3 { 1., 2., 3. };
    auto b = vec3 { 4., 5., 6. };

    auto add1 = add(a, b);
    auto sub1 = sub(a, b);
    auto mul1 = mul(a, 2.);
    auto mul2 = mul(2., a);
    auto mul3 = mul_by_elem(a, b);
    auto div1 = div(a, 2.);
    auto div2 = div(6., a);
    auto div3 = div_by_elem(b, a);

    EXPECT_TRUE(equals(     //
        add1,               //
        vec3 { 5., 7., 9. } //
        ));

    EXPECT_TRUE(equals(        //
        sub1,                  //
        vec3 { -3., -3., -3. } //
        ));

    EXPECT_TRUE(equals(     //
        mul1,               //
        vec3 { 2., 4., 6. } //
        ));

    EXPECT_TRUE(equals(     //
        mul2,               //
        vec3 { 2., 4., 6. } //
        ));

    EXPECT_TRUE(equals(       //
        mul3,                 //
        vec3 { 4., 10., 18. } //
        ));

    EXPECT_TRUE(equals(      //
        div1,                //
        vec3 { .5, 1., 1.5 } //
        ));

    EXPECT_TRUE(equals(     //
        div2,               //
        vec3 { 6., 3., 2. } //
        ));

    EXPECT_TRUE(equals(      //
        div3,                //
        vec3 { 4., 2.5, 2. } //
        ));
  }

  TEST(math, vector_dot_product_constexpr) {
    constexpr auto a = vec3 { 1, 2, 3 };
    constexpr auto b = vec3 { 4, 5, 6 };
    constexpr auto c = dot(a, b);

    EXPECT_DOUBLE_EQ(c, 32.);
  }

  TEST(math, vector_dot_product) {
    auto a = vec3 { 1, 2, 3 };
    auto b = vec3 { 4, 5, 6 };

    EXPECT_DOUBLE_EQ( //
        dot(a, b),    //
        32.           //
    );
  }

  TEST(math, vector_cross_product_constexpr) {
    constexpr auto a = vec3 { 1, 2, 3 };
    constexpr auto b = vec3 { 4, 5, 6 };
    constexpr auto c = cross(a, b);

    EXPECT_TRUE(equals(       //
        c,                    //
        vec3 { -3., 6., -3. } //
        ));
  }

  TEST(math, vector_cross_product) {
    constexpr auto a = vec3 { 1, 2, 3 };
    constexpr auto b = vec3 { 4, 5, 6 };

    EXPECT_TRUE(equals(       //
        cross(a, b),          //
        vec3 { -3., 6., -3. } //
        ));
  }

  TEST(math, vector_square_length_constexpr) {
    constexpr auto a = vec3 { 1, 2, 3 };
    constexpr auto l = square_length(a);

    EXPECT_DOUBLE_EQ(l, 14.);
  }

  TEST(math, vector_square_length) {
    auto a = vec3 { 1, 2, 3 };

    EXPECT_DOUBLE_EQ(     //
        square_length(a), //
        14.               //
    );
  }
}
