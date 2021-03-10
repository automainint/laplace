/*  test/unittests/m_matrix.test.cpp
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

#include "../../laplace/core/defs.h"
#include "../../laplace/math/basic.h"
#include "../../laplace/math/matrix.h"
#include "../../laplace/math/shortnames.h"
#include <gtest/gtest.h>
#include <random>

namespace laplace::test {
  using math::add, math::sub, math::equals,
      math::shortnames::mat4, std::default_random_engine,
      std::uniform_real_distribution;

  TEST(math, matrix_add_constexpr) {
    constexpr auto a = mat4 //
        { 1,  2,  3,  4,    //
          5,  6,  7,  8,    //
          -1, -2, -3, -4,   //
          -5, -6, -7, -8 };

    constexpr auto b = mat4 //
        { -1, -2, -3, -4,   //
          -5, -6, -7, -8,   //
          1,  2,  3,  4,    //
          5,  6,  7,  8 };

    constexpr auto c = mat4 //
        { 0, 0, 0, 0,       //
          0, 0, 0, 0,       //
          0, 0, 0, 0,       //
          0, 0, 0, 0 };

    constexpr auto d = add(a, b);

    EXPECT_TRUE(equals(c, d));
  }

  TEST(math, matrix_add) {
    auto a = mat4         //
        { 1,  2,  3,  4,  //
          5,  6,  7,  8,  //
          -1, -2, -3, -4, //
          -5, -6, -7, -8 };

    auto b = mat4         //
        { -1, -2, -3, -4, //
          -5, -6, -7, -8, //
          1,  2,  3,  4,  //
          5,  6,  7,  8 };

    auto c = mat4     //
        { 0, 0, 0, 0, //
          0, 0, 0, 0, //
          0, 0, 0, 0, //
          0, 0, 0, 0 };

    auto d = add(a, b);

    EXPECT_TRUE(equals(c, d));
  }

  TEST(math, matrix_sub_constexpr) {
    constexpr auto a = mat4 //
        { 1,  2,  3,  4,    //
          5,  6,  7,  8,    //
          -1, -2, -3, -4,   //
          -5, -6, -7, -8 };

    constexpr auto b = mat4 //
        { 0, 0, 0, 0,       //
          0, 0, 0, 0,       //
          0, 0, 0, 0,       //
          0, 0, 0, 0 };

    constexpr auto c = sub(a, a);

    EXPECT_TRUE(equals(b, c));
  }

  TEST(math, matrix_sub) {
    auto a = mat4         //
        { 1,  2,  3,  4,  //
          5,  6,  7,  8,  //
          -1, -2, -3, -4, //
          -5, -6, -7, -8 };

    auto b = mat4     //
        { 0, 0, 0, 0, //
          0, 0, 0, 0, //
          0, 0, 0, 0, //
          0, 0, 0, 0 };

    auto c = sub(a, a);

    EXPECT_TRUE(equals(b, c));
  }

  TEST(math, matrix_add_sub) {
    constexpr size_t test_count = 10;

    default_random_engine            rnd;
    uniform_real_distribution<float> dst(-100, 100);

    for (size_t i = 0; i < test_count; i++) {
      auto a = mat4 {};
      auto b = mat4 {};

      for (size_t j = 0; j < 16; j++) {
        a.v[j] = dst(rnd);
        b.v[j] = dst(rnd);
      }

      auto c = sub(add(a, b), b);

      EXPECT_TRUE(equals(a, c, 1e-5f));
    }
  }
}
