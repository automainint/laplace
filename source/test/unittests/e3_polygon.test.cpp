/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/polygon.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::polygon, engine::tridi::plane, engine::vec3i;

  TEST(engine, tridi_polygon_get_plane_empty) {
    auto foo = polygon {};
    auto bar = polygon { vec3i {} };
    auto sub = polygon { vec3i {}, vec3i {} };

    EXPECT_EQ(foo.get_plane(), plane {});
    EXPECT_EQ(bar.get_plane(), plane {});
    EXPECT_EQ(sub.get_plane(), plane {});
  }

  TEST(engine, tridi_polygon_get_plane_3) {
    auto foo = polygon { vec3i { -1, -1, -2 }, vec3i { -1, 5, -2 },
                         vec3i { 4, 1, -2 } };

    EXPECT_EQ(foo.get_plane().base.z(), -2);
    EXPECT_EQ(foo.get_plane().normal.x(), 0);
    EXPECT_EQ(foo.get_plane().normal.y(), 0);
    EXPECT_NE(foo.get_plane().normal.z(), 0);
  }
}
