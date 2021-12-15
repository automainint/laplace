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
  using ui::rect, ui::rectf, ui::compare, ui::contains,
      ui::intersects, ui::to_rect, ui::to_rectf;

  TEST(ui, rect_compare) {
    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = 40 },
        rect { .x = 10, .y = 20, .width = 30, .height = 40 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = -2 },
        rect { .x = 10, .y = 20, .width = -3, .height = -4 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = -2 },
        rect { .x = 10, .y = 20, .width = 30, .height = -4 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = 40 },
        rect { .x = 10, .y = 20, .width = -3, .height = 40 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = 40 },
        rect { .x = 10, .y = 20, .width = -3, .height = -4 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = -2 },
        rect { .x = 10, .y = 20, .width = -3, .height = 40 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = -2 },
        rect { .x = -10, .y = -20, .width = -3, .height = 40 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = -2 },
        rect { .x = -10, .y = -20, .width = 30, .height = -4 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = 0 },
        rect { .x = -10, .y = -20, .width = 30, .height = -4 }));

    EXPECT_TRUE(compare(
        rect { .x = 10, .y = 20, .width = -1, .height = -2 },
        rect { .x = -10, .y = -20, .width = 0, .height = 40 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 20, .height = 40 },
        rect { .x = -10, .y = -20, .width = 20, .height = 40 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 20, .height = 30 },
        rect { .x = 10, .y = 20, .width = 40, .height = 50 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = 40 },
        rect { .x = 10, .y = 20, .width = 30, .height = 50 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = 40 },
        rect { .x = 10, .y = 20, .width = 50, .height = 40 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = 40 },
        rect { .x = 10, .y = 50, .width = 30, .height = 40 }));

    EXPECT_FALSE(compare(
        rect { .x = 10, .y = 20, .width = 30, .height = 40 },
        rect { .x = 50, .y = 20, .width = 30, .height = 40 }));
  }

  TEST(ui, rect_contains_point) {
    auto const a = rect { 10, 5, 100, 200 };

    EXPECT_TRUE(contains(a, 10, 5));
    EXPECT_TRUE(contains(a, 20, 20));
    EXPECT_TRUE(contains(a, 109, 5));
    EXPECT_TRUE(contains(a, 10, 204));
    EXPECT_TRUE(contains(a, 109, 204));
    EXPECT_FALSE(contains(a, 9, 5));
    EXPECT_FALSE(contains(a, 10, 4));
    EXPECT_FALSE(contains(a, 110, 204));
    EXPECT_FALSE(contains(a, 109, 205));
    EXPECT_FALSE(contains(a, 5, 10));
    EXPECT_FALSE(contains(a, 110, 205));
    EXPECT_FALSE(contains(a, 109, 205));
    EXPECT_FALSE(contains(a, 110, 204));
    EXPECT_FALSE(contains(a, 200, 300));
  }

  TEST(ui, rect_intersects_rect) {
    EXPECT_TRUE(intersects(rect { 20, 10, 50, 100 },
                           rect { 60, 90, 20, 20 }));

    EXPECT_TRUE(
        intersects(rect { 10, 20, 30, 40 }, rect { 1, 1, 10, 20 }));

    EXPECT_TRUE(
        intersects(rect { 10, 20, 30, 40 }, rect { 39, 59, 10, 10 }));

    EXPECT_FALSE(
        intersects(rect { 10, 20, 30, 40 }, rect { 1, 0, 10, 20 }));

    EXPECT_FALSE(
        intersects(rect { 10, 20, 30, 40 }, rect { 0, 1, 10, 20 }));

    EXPECT_FALSE(
        intersects(rect { 10, 20, 30, 40 }, rect { 40, 59, 10, 10 }));

    EXPECT_FALSE(
        intersects(rect { 10, 20, 30, 40 }, rect { 39, 60, 10, 10 }));

    EXPECT_FALSE(intersects(rect { 20, 10, 50, 100 },
                            rect { 10, 200, 50, 50 }));
  }

  TEST(ui, rect_contains_rect) {
    EXPECT_TRUE(contains(rect { 10, 10, 100, 200 },
                         rect { 20, 20, 90, 190 }));

    EXPECT_TRUE(
        contains(rect { 10, 10, 100, 200 }, rect { 10, 10, 20, 20 }));

    EXPECT_TRUE(contains(rect { 10, 10, 100, 200 },
                         rect { 90, 190, 20, 20 }));

    EXPECT_FALSE(
        contains(rect { 10, 10, 100, 200 }, rect { 9, 10, 20, 20 }));

    EXPECT_FALSE(
        contains(rect { 10, 10, 100, 200 }, rect { 10, 9, 20, 20 }));

    EXPECT_FALSE(contains(rect { 10, 10, 100, 200 },
                          rect { 91, 190, 20, 20 }));

    EXPECT_FALSE(contains(rect { 10, 10, 100, 200 },
                          rect { 90, 191, 20, 20 }));

    EXPECT_FALSE(
        contains(rect { 10, 10, 100, 200 }, rect { 9, 10, 50, 50 }));
  }

  TEST(ui, rect_to_rectf) {
    auto const a = to_rectf(
        rect { .x = 1, .y = 2, .width = 3, .height = 4 });
    EXPECT_FLOAT_EQ(a.x, 1.f);
    EXPECT_FLOAT_EQ(a.y, 2.f);
    EXPECT_FLOAT_EQ(a.width, 3.f);
    EXPECT_FLOAT_EQ(a.height, 4.f);
  }

  TEST(ui, rectf_to_rect) {
    auto const a = to_rect(
        rectf { .x = 1.f, .y = 2.f, .width = 3.f, .height = 4.f });
    EXPECT_EQ(a.x, 1);
    EXPECT_EQ(a.y, 2);
    EXPECT_EQ(a.width, 3);
    EXPECT_EQ(a.height, 4);
  }
}
