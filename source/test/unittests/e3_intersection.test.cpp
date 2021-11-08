/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/box.h"
#include "../../laplace/engine/tridi/cylinder.h"
#include "../../laplace/engine/tridi/intersection.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::intersects, engine::tridi::box,
      engine::tridi::cylinder, engine::vec3i;

  TEST(engine, tridi_intersects_box) {
    auto foo = box { .min = { -1, -1, -1 }, .max = { 1, 2, 3 } };
    auto bar = box { .min = { 1, 2, 3 }, .max = { 4, 5, 6 } };
    auto bus = box { .min = { 2, 3, 4 }, .max = { 3, 4, 5 } };

    EXPECT_TRUE(intersects(foo, bar));
    EXPECT_TRUE(intersects(bar, foo));
    EXPECT_TRUE(intersects(bar, bus));
    EXPECT_TRUE(intersects(bus, bar));
    EXPECT_FALSE(intersects(foo, bus));
    EXPECT_FALSE(intersects(bus, foo));
  }

  TEST(engine, tridi_intersects_box_cylinder) {
    auto foo = box { .min = { -1, -1, -2 }, .max = { 1, 1, 2 } };

    auto bar = cylinder { .base   = { 0, 0, 2 },
                          .radius = 1,
                          .height = 1 };

    auto bus = cylinder { .base   = { 0, 0, 3 },
                          .radius = 1,
                          .height = 1 };

    auto oof = cylinder { .base   = { -1, 0, 1 },
                          .radius = 1,
                          .height = 1 };

    EXPECT_TRUE(intersects(foo, bar));
    EXPECT_TRUE(intersects(foo, oof));
    EXPECT_FALSE(intersects(foo, bus));
  }
}
