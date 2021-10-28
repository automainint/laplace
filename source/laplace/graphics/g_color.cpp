/*  laplace/graphics/g_color.cpp
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

#include "color.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace laplace::graphics {
  using std::numeric_limits, std::min, std::max;

  void rgb_to_hsl(const float *rgb, float *hsl) {
    constexpr auto feps = numeric_limits<float>::epsilon();

    if (rgb && hsl) {
      float x_max     = max(max(rgb[0], rgb[1]), rgb[2]);
      float x_min     = min(min(rgb[0], rgb[1]), rgb[2]);
      float chroma    = x_max - x_min;
      float lightness = (x_max + x_min) / 2.f;

      float hue        = 0.f;
      float saturation = 0.f;

      if (chroma > feps) {
        if (rgb[0] >= rgb[1] && rgb[0] >= rgb[2]) {
          hue = ((rgb[1] - rgb[2]) / chroma) / 6.f;
        } else if (rgb[1] >= rgb[2]) {
          hue = (2.f + (rgb[2] - rgb[0]) / chroma) / 6.f;
        } else {
          hue = (4.f + (rgb[0] - rgb[1]) / chroma) / 6.f;
        }
      }

      float l_opposite = 1.f - lightness;

      if (lightness > feps && l_opposite > feps) {
        saturation = (x_max - lightness) / min(lightness, l_opposite);
      }

      hsl[0] = hue;
      hsl[1] = saturation;
      hsl[2] = lightness;
    }
  }

  void hsl_to_rgb(const float *hsl, float *rgb) {
    if (hsl && rgb) {
      float alpha     = hsl[1] * min(hsl[2], 1.f - hsl[2]);
      float hue12     = hsl[0] * 12.f;
      float lightness = hsl[2];

      auto f = [hue12, lightness, alpha](float n) -> float {
        float k = fmod(n + hue12, 12.f);
        return lightness -
               alpha * max(-1.f, min(min(k - 3.f, 9.f - k), 1.f));
      };

      rgb[0] = f(0);
      rgb[1] = f(8);
      rgb[2] = f(4);
    }
  }

  auto convert_color_channel(float x) -> uint8_t {
    return static_cast<uint8_t>(
        max(0, min(static_cast<int>(x * 255 + .5f), 255)));
  }

  auto convert_color_channel(uint8_t x) -> float {
    return max(0.f, min(static_cast<float>(x) / 255, 1.f));
  }

  void convert_rgb(const float *f_rgb, uint8_t *u8_rgb) {
    if (f_rgb && u8_rgb) {
      u8_rgb[0] = convert_color_channel(f_rgb[0]);
      u8_rgb[1] = convert_color_channel(f_rgb[1]);
      u8_rgb[2] = convert_color_channel(f_rgb[2]);
    }
  }

  void convert_rgb(const uint8_t *u8_rgb, float *f_rgb) {
    if (f_rgb && u8_rgb) {
      f_rgb[0] = convert_color_channel(u8_rgb[0]);
      f_rgb[1] = convert_color_channel(u8_rgb[1]);
      f_rgb[2] = convert_color_channel(u8_rgb[2]);
    }
  }

  void convert_rgba(const float *f_rgba, uint8_t *u8_rgba) {
    if (f_rgba && u8_rgba) {
      u8_rgba[0] = convert_color_channel(f_rgba[0]);
      u8_rgba[1] = convert_color_channel(f_rgba[1]);
      u8_rgba[2] = convert_color_channel(f_rgba[2]);
      u8_rgba[3] = convert_color_channel(f_rgba[3]);
    }
  }

  void convert_rgba(const uint8_t *u8_rgba, float *f_rgba) {
    if (f_rgba && u8_rgba) {
      f_rgba[0] = convert_color_channel(u8_rgba[0]);
      f_rgba[1] = convert_color_channel(u8_rgba[1]);
      f_rgba[2] = convert_color_channel(u8_rgba[2]);
      f_rgba[3] = convert_color_channel(u8_rgba[3]);
    }
  }
}
