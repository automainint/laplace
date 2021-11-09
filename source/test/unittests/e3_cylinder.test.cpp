/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/cylinder.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::cylinder, engine::tridi::box, engine::vec3i;

  TEST(engine, tridi_cylinder_is_empty) {
    EXPECT_TRUE(cylinder {}.is_empty());
    
    auto const foo = cylinder {.radius = 1, .height = -1};
    auto const bar = cylinder {.radius = -1, .height = 1};

    EXPECT_TRUE(foo.is_empty());
    EXPECT_TRUE(bar.is_empty());
  }

  TEST(engine, tridi_cylinder_get_bounds) {
    auto const foo = cylinder { .base   = { 0, 0, -1 },
                                .radius = 1,
                                .height = 2 };

    auto const bar = box { .min = { -1, -1, -1 },
                           .max = { 1, 1, 1 } };

    EXPECT_EQ(foo.get_bounds(), bar);
  }

  TEST(engine, tridi_cylinder_contains_flat) {
    auto const foo = cylinder { .base   = { 0, 0, -1 },
                                .radius = 1,
                                .height = 2 };

    EXPECT_TRUE(foo.contains_flat(vec3i { 0, 0, 0 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 1, 0, 4 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 0, 1, -5 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 1, 1, 0 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 3, 0, 0 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { -3, 0, 0 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 0, 4, 0 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 0, -4, 0 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 0, 0, 5 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 0, 0, -5 }));
  }

  TEST(engine, tridi_cylinder_contains) {
    auto const foo = cylinder { .base   = { 0, 0, -1 },
                                .radius = 1,
                                .height = 2 };

    EXPECT_TRUE(foo.contains(vec3i { 0, 0, 0 }));
    EXPECT_FALSE(foo.contains(vec3i { 0, -5, 0 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 0, 4 }));
    EXPECT_FALSE(foo.contains(vec3i { 0, 1, -5 }));
  }
}
