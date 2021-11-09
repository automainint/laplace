/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/plane.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::vec3i, engine::tridi::raw_normal;

  TEST(engine, tridi_raw_normal) {
    auto a = vec3i { 3, -1, -1 };
    auto b = vec3i { 3, 2, -1 };
    auto c = vec3i { 3, 3, 3 };

    EXPECT_NE(raw_normal(a, b, c).x(), 0);
    EXPECT_EQ(raw_normal(a, b, c).y(), 0);
    EXPECT_EQ(raw_normal(a, b, c).z(), 0);
  }
}
