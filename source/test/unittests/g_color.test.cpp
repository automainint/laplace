/*  Copyright (c) 2021 Mitya Selivanov
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
  using graphics::hsl_to_rgb, graphics::rgb_to_hsl,
      graphics::convert_rgb, graphics::convert_rgba;

  TEST(graphics, color_rgb_hsl_rgb) {
    float const src_rgb[3] = { .1f, .2f, .3f };
    float       dst_hsl[3] = {};
    float       dst_rgb[3] = {};

    rgb_to_hsl(src_rgb, dst_hsl, get_global_log());
    hsl_to_rgb(dst_hsl, dst_rgb, get_global_log());

    EXPECT_FLOAT_EQ(src_rgb[0], dst_rgb[0]);
    EXPECT_FLOAT_EQ(src_rgb[1], dst_rgb[1]);
    EXPECT_FLOAT_EQ(src_rgb[2], dst_rgb[2]);
  }

  TEST(graphics, color_hsl_rgb_hsl) {
    float const src_hsl[3] = { .1f, .2f, .3f };
    float       dst_rgb[3] = {};
    float       dst_hsl[3] = {};

    hsl_to_rgb(src_hsl, dst_rgb, get_global_log());
    rgb_to_hsl(dst_rgb, dst_hsl, get_global_log());

    EXPECT_FLOAT_EQ(src_hsl[0], dst_hsl[0]);
    EXPECT_FLOAT_EQ(src_hsl[1], dst_hsl[1]);
    EXPECT_FLOAT_EQ(src_hsl[2], dst_hsl[2]);
  }

  TEST(graphics, color_convert_rgb_1) {
    float   src[3] = { .0f, .5f, 1.f };
    uint8_t dst[3] = {};

    convert_rgb(src, dst, get_global_log());

    EXPECT_EQ(dst[0], 0);
    EXPECT_EQ(dst[1], 128);
    EXPECT_EQ(dst[2], 255);
  }

  TEST(graphics, color_convert_rgb_2) {
    uint8_t src[3] = { 0, 128, 255 };
    float   dst[3] = {};

    convert_rgb(src, dst, get_global_log());

    EXPECT_FLOAT_EQ(dst[0], .0f);
    EXPECT_FLOAT_EQ(dst[1], 128.f / 255);
    EXPECT_FLOAT_EQ(dst[2], 1.f);
  }

  TEST(graphics, color_convert_rgba_1) {
    float   src[4] = { .0f, .5f, 1.f, 1.f };
    uint8_t dst[4] = {};

    convert_rgba(src, dst, get_global_log());

    EXPECT_EQ(dst[0], 0);
    EXPECT_EQ(dst[1], 128);
    EXPECT_EQ(dst[2], 255);
    EXPECT_EQ(dst[3], 255);
  }

  TEST(graphics, color_convert_rgba_2) {
    uint8_t src[4] = { 0, 128, 255, 255 };
    float   dst[4] = {};

    convert_rgba(src, dst, get_global_log());

    EXPECT_FLOAT_EQ(dst[0], .0f);
    EXPECT_FLOAT_EQ(dst[1], 128.f / 255);
    EXPECT_FLOAT_EQ(dst[2], 1.f);
    EXPECT_FLOAT_EQ(dst[3], 1.f);
  }
}
