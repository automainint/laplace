/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/octree.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::octree, engine::tridi::triangle;

  TEST(engine, tridi_octree_is_empty) {
    auto foo = octree {};

    auto bar = octree { .bounds   = { .min = { -1, -1, -1 },
                                    .max = { 1, 1, 1 } },
                        .children = sl::vector<triangle> {} };

    auto sub = octree { .bounds   = { .min = { -1, -1, -1 },
                                    .max = { 1, 1, 1 } },
                        .children = sl::vector<octree> {} };

    auto bus = octree { .bounds   = { .min = { -1, -1, -1 },
                                    .max = { 1, 1, 1 } },
                        .children = sl::vector<triangle>(1) };

    auto oof = octree { .bounds   = { .min = { -1, -1, -1 },
                                    .max = { 1, 1, 1 } },
                        .children = sl::vector<octree>(1) };

    auto rab = octree { .bounds   = { .min = { -1, -1, -1 },
                                    .max = { 1, 1, 1 } },
                        .children = sl::vector<octree>(1, bus) };

    EXPECT_TRUE(foo.is_empty());
    EXPECT_TRUE(bar.is_empty());
    EXPECT_TRUE(sub.is_empty());
    EXPECT_FALSE(bus.is_empty());
    EXPECT_TRUE(oof.is_empty());
    EXPECT_FALSE(rab.is_empty());
  }
}
