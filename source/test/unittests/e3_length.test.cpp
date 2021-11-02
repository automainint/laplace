/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/length.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::vec3i, engine::tridi::length;

  TEST(engine, eval_length) {
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
