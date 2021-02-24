/*  test/unittests/ui_rect.test.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/ui/rect.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using ui::rect, ui::contains, ui::intersects;

  TEST(ui, rect_contains_point) {
    rect a { 10, 5, 100, 200 };

    EXPECT_TRUE(contains(a, 10, 5));
    EXPECT_TRUE(contains(a, 20, 20));
    EXPECT_TRUE(contains(a, 109, 5));
    EXPECT_TRUE(contains(a, 10, 204));
    EXPECT_TRUE(contains(a, 109, 204));
    EXPECT_FALSE(contains(a, 5, 10));
    EXPECT_FALSE(contains(a, 110, 205));
    EXPECT_FALSE(contains(a, 109, 205));
    EXPECT_FALSE(contains(a, 110, 204));
    EXPECT_FALSE(contains(a, 200, 300));
  }

  TEST(ui, rect_intersects_rect) {
    rect a { 20, 10, 50, 100 };
    rect b { 60, 90, 20, 20 };
    rect c { 10, 200, 50, 50 };

    EXPECT_TRUE(intersects(a, b));
    EXPECT_FALSE(intersects(a, c));
  }

  TEST(ui, rect_contains_rect) {
    rect a { 10, 10, 100, 200 };
    rect b { 20, 20, 90, 190 };
    rect c { 9, 10, 50, 50 };

    EXPECT_TRUE(contains(a, b));
    EXPECT_FALSE(contains(a, c));
  }
}
