/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/sphere.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::sphere, engine::tridi::box, engine::vec3i;

  TEST(engine, tridi_sphere_is_empty) {
    EXPECT_TRUE(sphere {}.is_empty());
  }

  TEST(engine, tridi_sphere_get_bounds) {
    auto const foo = sphere { .center = { 3, 3, 3 }, .radius = 2 };
    auto const bar = box { .min = { 1, 1, 1 }, .max = { 5, 5, 5 } };

    EXPECT_EQ(foo.get_bounds(), bar);
  }

  TEST(engine, tridi_sphere_contains) {
    auto const foo = sphere { .center = { 3, 3, 3 }, .radius = 2 };

    EXPECT_TRUE(foo.contains(vec3i { 3, 3, 3 }));
    EXPECT_TRUE(foo.contains(vec3i { 1, 3, 3 }));
    EXPECT_TRUE(foo.contains(vec3i { 3, 1, 3 }));
    EXPECT_TRUE(foo.contains(vec3i { 3, 3, 1 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 1, 3 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 2, 2 }));
    EXPECT_FALSE(foo.contains(vec3i { 6, 6, 6 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 6, 6 }));
    EXPECT_FALSE(foo.contains(vec3i { 6, 1, 6 }));
    EXPECT_FALSE(foo.contains(vec3i { 6, 6, 1 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 1, 6 }));
    EXPECT_FALSE(foo.contains(vec3i { 1, 6, 1 }));
    EXPECT_FALSE(foo.contains(vec3i { 6, 1, 1 }));
  }
}
