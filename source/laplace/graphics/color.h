/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_graphics_color_h
#define laplace_graphics_color_h

#include <cstdint>

namespace laplace::graphics {
  void rgb_to_hsl(float const *rgb, float *hsl) noexcept;
  void hsl_to_rgb(float const *hsl, float *rgb) noexcept;

  [[nodiscard]] auto convert_color_channel(float x) noexcept
      -> uint8_t;
  [[nodiscard]] auto convert_color_channel(uint8_t x) noexcept
      -> float;

  void convert_rgb(float const *f_rgb, uint8_t *u8_rgb) noexcept;
  void convert_rgb(uint8_t const *u8_rgb, float *f_rgb) noexcept;

  void convert_rgba(float const *f_rgba, uint8_t *u8_rgba) noexcept;
  void convert_rgba(uint8_t const *u8_rgba, float *f_rgba) noexcept;
}

#endif
