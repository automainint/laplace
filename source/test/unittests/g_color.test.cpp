/*  test/unittests/g_color.test.cpp
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

#include "../../laplace/graphics/color.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using graphics::hsl_to_rgb, graphics::rgb_to_hsl;

  TEST(graphics, color_rgb_hsl_rgb) {
    float const src_rgb[3] = { .1f, .2f, .3f };
    float       dst_hsl[3] = {};
    float       dst_rgb[3] = {};

    rgb_to_hsl(src_rgb, dst_hsl);
    hsl_to_rgb(dst_hsl, dst_rgb);

    EXPECT_FLOAT_EQ(src_rgb[0], dst_rgb[0]);
    EXPECT_FLOAT_EQ(src_rgb[1], dst_rgb[1]);
    EXPECT_FLOAT_EQ(src_rgb[2], dst_rgb[2]);
  }

  TEST(graphics, color_hsl_rgb_hsl) {
    float const src_hsl[3] = { .1f, .2f, .3f };
    float       dst_rgb[3] = {};
    float       dst_hsl[3] = {};

    hsl_to_rgb(src_hsl, dst_rgb);
    rgb_to_hsl(dst_rgb, dst_hsl);

    EXPECT_FLOAT_EQ(src_hsl[0], dst_hsl[0]);
    EXPECT_FLOAT_EQ(src_hsl[1], dst_hsl[1]);
    EXPECT_FLOAT_EQ(src_hsl[2], dst_hsl[2]);
  }
}
