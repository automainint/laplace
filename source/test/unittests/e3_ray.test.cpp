/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/ray.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::ray, engine::vec3i;

  TEST(engine, tridi_ray_point) {
    auto foo = ray { .base      = vec3i { 0, 0, 20 },
                     .direction = vec3i { 0, 1, 0 } };

    auto bar = ray { .base      = vec3i { 1, 2, 3 },
                     .direction = vec3i { 0, 0, 0 } };

    auto a = vec3i { 0, 10, 20 };
    auto b = vec3i { 0, 20, 20 };

    EXPECT_EQ(foo.point(0), foo.base);
    EXPECT_EQ(foo.point(10), a);
    EXPECT_EQ(foo.point(20), b);
    EXPECT_EQ(bar.point(0), bar.base);
    EXPECT_EQ(bar.point(10), bar.base);
    EXPECT_EQ(bar.point(30), bar.base);
  }

  TEST(engine, tridi_ray_square_distance_point) {
    auto foo = ray { .base      = vec3i { 2, 2, 2 },
                     .direction = { 5, 0, 0 } };

    auto bar = vec3i { 10, 1, 2 };
    auto sub = vec3i { 12, 2, 0 };

    EXPECT_EQ(foo.square_distance(foo.base), 0);
    EXPECT_EQ(foo.square_distance(bar), 1);
    EXPECT_EQ(foo.square_distance(sub), 4);
  }

  TEST(engine, tridi_ray_square_distance_ray) {
    auto foo = ray { .base      = vec3i { -1, -1, -1 },
                     .direction = vec3i { 1, 0, 0 } };

    auto bar = ray { .base      = vec3i { 0, 0, 0 },
                     .direction = vec3i { 3, 4, 0 } };

    auto bus = ray { .base      = vec3i { 0, 0, 0 },
                     .direction = vec3i { 0, 0, 0 } };

    EXPECT_EQ(foo.square_distance(bar), 1);
    EXPECT_LT(foo.square_distance(bus), 0);
    EXPECT_LT(bus.square_distance(bar), 0);
  }
}
