/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_GRAPHICS_COLOR_H
#define LAPLACE_GRAPHICS_COLOR_H

#include "../core/log.h"
#include <cstdint>

namespace laplace::graphics {
  void rgb_to_hsl(float const *rgb, float *hsl,
                  log_handler log) noexcept;
  void hsl_to_rgb(float const *hsl, float *rgb,
                  log_handler log) noexcept;

  [[nodiscard]] auto convert_color_channel(float x) noexcept
      -> uint8_t;
  [[nodiscard]] auto convert_color_channel(uint8_t x) noexcept
      -> float;

  void convert_rgb(float const *f_rgb, uint8_t *u8_rgb,
                   log_handler log) noexcept;
  void convert_rgb(uint8_t const *u8_rgb, float *f_rgb,
                   log_handler log) noexcept;

  void convert_rgba(float const *f_rgba, uint8_t *u8_rgba,
                    log_handler log) noexcept;
  void convert_rgba(uint8_t const *u8_rgba, float *f_rgba,
                    log_handler log) noexcept;
}

#endif
