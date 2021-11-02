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
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::vec3i, engine::tridi::box;

  TEST(engine, eval_box_is_empty) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 1, 2, 3 } };
    auto const bar = box { .min = { 2, 1, 3 }, .max = { 1, 2, 3 } };
    auto const sub = box { .min = { 1, 3, 3 }, .max = { 1, 2, 3 } };
    auto const bus = box { .min = { 1, 2, 3 }, .max = { 1, 2, 2 } };

    EXPECT_TRUE(box {}.is_empty());
    EXPECT_FALSE(foo.is_empty());
    EXPECT_TRUE(bar.is_empty());
    EXPECT_TRUE(sub.is_empty());
    EXPECT_TRUE(bus.is_empty());
  }

  TEST(engine, eval_box_append) {
    EXPECT_FALSE(box {}.append(vec3i {}).is_empty());
  }
}
