/*  laplace/graphics/pixel.impl.h
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

#ifndef laplace_graphics_pixel_impl_h
#define laplace_graphics_pixel_impl_h

namespace laplace::graphics {
  constexpr auto rgba(uint8_t r, uint8_t g, uint8_t b,
                      uint8_t a) noexcept -> pixel {
    return pixel { .red = r, .green = g, .blue = b, .alpha = a };
  }
}

#endif
