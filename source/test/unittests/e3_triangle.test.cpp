/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/triangle.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::triangle, engine::tridi::box, engine::vec3i;

  TEST(engine, tridi_triangle_get_center) {
    auto foo = triangle { vec3i { 0, 0, 2 }, vec3i { 2, 0, 0 },
                          vec3i { 0, 2, 0 } };

    auto bar = vec3i { 1, 1, 1 };

    EXPECT_EQ(foo.get_center(), bar);
  }

  TEST(engine, tridi_triangle_get_mass_center) {
    auto foo = triangle { vec3i { 0, 0, 3 }, vec3i { 3, 0, 0 },
                          vec3i { 0, 3, 0 } };

    auto bar = vec3i { 1, 1, 1 };

    EXPECT_EQ(foo.get_mass_center(), bar);
  }

  TEST(engine, tridi_triangle_get_plane_x) {
    auto foo = triangle { vec3i { 1, -1, 1 }, vec3i { 1, 1, 1 },
                          vec3i { 1, 0, 2 } };

    EXPECT_EQ(foo.get_plane().base.x(), 1);
    EXPECT_NE(foo.get_plane().normal.x(), 0);
    EXPECT_EQ(foo.get_plane().normal.y(), 0);
    EXPECT_EQ(foo.get_plane().normal.z(), 0);
  }

  TEST(engine, tridi_triangle_get_plane_y) {
    auto foo = triangle { vec3i { -1, 1, 0 }, vec3i { 1, 1, 0 },
                          vec3i { 0, 1, 2 } };

    EXPECT_EQ(foo.get_plane().base.y(), 1);
    EXPECT_EQ(foo.get_plane().normal.x(), 0);
    EXPECT_NE(foo.get_plane().normal.y(), 0);
    EXPECT_EQ(foo.get_plane().normal.z(), 0);
  }

  TEST(engine, tridi_triangle_get_plane_z) {
    auto foo = triangle { vec3i { -1, -1, 1 }, vec3i { -1, 1, 1 },
                          vec3i { 2, 0, 1 } };

    EXPECT_EQ(foo.get_plane().base.z(), 1);
    EXPECT_EQ(foo.get_plane().normal.x(), 0);
    EXPECT_EQ(foo.get_plane().normal.y(), 0);
    EXPECT_NE(foo.get_plane().normal.z(), 0);
  }

  TEST(engine, tridi_triangle_get_flat_center) {
    EXPECT_EQ(triangle {}.get_flat_center(-1), 0);
    EXPECT_EQ(triangle {}.get_flat_center(3), 0);

    auto foo = triangle { vec3i { 2, 0, 0 }, vec3i { 0, 4, 0 },
                          vec3i { 0, 0, 6 } };

    EXPECT_EQ(foo.get_flat_center(0), 1);
    EXPECT_EQ(foo.get_flat_center(1), 2);
    EXPECT_EQ(foo.get_flat_center(2), 3);
  }

  TEST(engine, tridi_triangle_get_bounds) {
    auto foo = triangle { vec3i { -2, 0, 0 }, vec3i { 0, 4, 0 },
                          vec3i { 0, 0, 6 } };

    auto bar = box { .min = { -2, 0, 0 }, .max = { 0, 4, 6 } };

    EXPECT_EQ(foo.get_bounds(), bar);
  }

  TEST(engine, tridi_triangle_plane_raw_normal) {
    auto foo = triangle { vec3i { -1, 2, -1 }, vec3i { 3, 2, 0 },
                          vec3i { 1, 2, 3 } };

    auto bar = foo.plane_raw_normal();

    EXPECT_EQ(bar.x(), 0);
    EXPECT_NE(bar.y(), 0);
    EXPECT_EQ(bar.z(), 0);
  }

  TEST(engine, tridi_triangle_orientation) {
    auto foo = triangle { vec3i { 0, 2, 2 }, vec3i { 2, 0, 2 },
                          vec3i { 2, 2, 0 } };

    auto x = vec3i { -1, -1, -1 };
    auto y = vec3i { 4, 4, 4 };

    EXPECT_NE(foo.orientation(x) > 0, foo.orientation(y) > 0);
  }

  TEST(engine, tridi_triangle_plane_square_distance) {
    auto foo = triangle { vec3i { -2, -2, 4 }, vec3i { 2, -2, 4 },
                          vec3i { 0, 2, 4 } };

    auto x = vec3i { -4, 0, 8 };
    auto y = vec3i { 8, 0, 12 };

    EXPECT_EQ(foo.plane_square_distance(x), -16);
    EXPECT_EQ(foo.plane_square_distance(y), -64);
  }

  TEST(engine, tridi_triangle_plane_abs_square_distance) {
    auto foo = triangle { vec3i { -2, 4, -3 }, vec3i { -2, 4, 3 },
                          vec3i { 0, 4, 3 } };

    auto x = vec3i { -4, 8, 1 };
    auto y = vec3i { 8, 12, 1 };

    EXPECT_EQ(foo.plane_abs_square_distance(x), 16);
    EXPECT_EQ(foo.plane_abs_square_distance(y), 64);
  }

  TEST(engine, tridi_triangle_contains_flat) {
    auto foo = triangle { vec3i { 10, 10, 1 }, vec3i { 20, 5, 1 },
                          vec3i { 18, 15, 1 } };

    auto x = vec3i { -5, 0, 1 };
    auto y = vec3i { 15, 9, 1 };

    EXPECT_FALSE(foo.contains_flat(x));
    EXPECT_TRUE(foo.contains_flat(y));
  }
}
