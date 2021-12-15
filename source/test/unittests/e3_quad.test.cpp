/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/quad.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::quad, engine::tridi::box, engine::vec3i;

  TEST(engine, tridi_quad_get_plane) {
    auto foo = quad { vec3i { 0, 0, -2 }, vec3i { 0, 4, -2 },
                      vec3i { 4, 4, -2 }, vec3i { 4, 0, -2 } };

    EXPECT_EQ(foo.get_plane().base.z(), -2);
    EXPECT_EQ(foo.get_plane().normal.x(), 0);
    EXPECT_EQ(foo.get_plane().normal.y(), 0);
  }

  TEST(engine, tridi_quad_get_bounds) {
    auto foo = quad { vec3i { 0, 0, -2 }, vec3i { 0, 4, 2 },
                      vec3i { 4, 4, -2 }, vec3i { 4, 0, 2 } };

    auto bar = box { .min = { 0, 0, -2 }, .max = { 4, 4, 2 } };

    EXPECT_EQ(foo.get_bounds(), bar);
  }

  TEST(engine, tridi_quad_contains_flat) {
    auto foo = quad { vec3i { -2, -2, 3 }, vec3i { -2, 2, 3 },
                      vec3i { 2, 2, -3 }, vec3i { 2, -2, -3 } };

    EXPECT_FALSE(foo.contains_flat(vec3i { -4, -4, 6 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { -4, 2, 6 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 2, 2, -6 }));
    EXPECT_FALSE(foo.contains_flat(vec3i { 2, -4, -6 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { -2, -2, 3 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { -2, 2, 3 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 2, 2, -3 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 2, -2, -3 }));
    EXPECT_TRUE(foo.contains_flat(vec3i { 0, 0, 0 }));
  }
}
