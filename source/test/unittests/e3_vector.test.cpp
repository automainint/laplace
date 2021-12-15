/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/vector.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::vec3i, engine::tridi::length, engine::tridi::add,
      engine::tridi::sub, engine::tridi::mul, engine::tridi::div;

  TEST(engine, tridi_vector_add) {
    auto foo = vec3i { 1, 2, 3 };
    auto bar = vec3i { 4, 5, 6 };
    auto bus = vec3i { 5, 7, 9 };

    EXPECT_EQ(add(foo, bar), bus);
  }

  TEST(engine, tridi_vector_sub) {
    auto foo = vec3i { 1, 2, 3 };
    auto bar = vec3i { 4, 5, 6 };
    auto bus = vec3i { -3, -3, -3 };

    EXPECT_EQ(sub(foo, bar), bus);
  }

  TEST(engine, tridi_vector_mul) {
    auto foo = vec3i { 1, 2, 3 };
    auto bar = vec3i { 2, 4, 6 };

    EXPECT_EQ(mul(foo, 2), bar);
    EXPECT_EQ(mul(2, foo), bar);
  }

  TEST(engine, tridi_vector_div) {
    auto foo = vec3i { -2, 4, -8 };
    auto bar = vec3i { -1, 2, -4 };
    auto bus = vec3i { -4, 2, -1 };

    EXPECT_EQ(div(foo, 2), bar);
    EXPECT_EQ(div(8, foo), bus);
  }

  TEST(engine, tridi_vector_length) {
    EXPECT_EQ(length(vec3i { 10, 0, 0 }), 10);
    EXPECT_EQ(length(vec3i { 0, 10, 0 }), 10);
    EXPECT_EQ(length(vec3i { 0, 0, 10 }), 10);
    EXPECT_EQ(length(vec3i { -10, 0, 0 }), 10);
    EXPECT_EQ(length(vec3i { 0, -10, 0 }), 10);
    EXPECT_EQ(length(vec3i { 0, 0, -10 }), 10);
    EXPECT_EQ(length(vec3i { 3, 4, 0 }), 5);
    EXPECT_EQ(length(vec3i { 0, 3, 4 }), 5);
    EXPECT_EQ(length(vec3i { 4, 0, 3 }), 5);
    EXPECT_EQ(length(vec3i { 3, -4, 0 }), 5);
    EXPECT_EQ(length(vec3i { 0, -3, 4 }), 5);
    EXPECT_EQ(length(vec3i { -4, 0, 3 }), 5);
    EXPECT_EQ(length(vec3i { 2, 2, 1 }), 3);
    EXPECT_EQ(length(vec3i { 2, 1, -2 }), 3);
    EXPECT_EQ(length(vec3i { 300, 0, -400 }), 500);
  }
}
