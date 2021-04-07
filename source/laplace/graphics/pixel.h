/*  laplace/graphics/pixel.h
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

#ifndef laplace_graphics_pixel_h
#define laplace_graphics_pixel_h

#include <cstdint>
#include <vector>

namespace laplace::graphics {
  struct pixel {
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;
    uint8_t alpha = 0;
  };

  using cref_pixel  = const pixel &;
  using vpixel      = std::vector<pixel>;
  using cref_vpixel = const vpixel &;

  [[nodiscard]] constexpr auto rgba(uint8_t r, uint8_t g, uint8_t b,
                                    uint8_t a) noexcept -> pixel;
}

#include "pixel.impl.h"

#endif
