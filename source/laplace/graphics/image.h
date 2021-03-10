/*  laplace/graphics/image.h
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

#ifndef laplace_graphics_image_h
#define laplace_graphics_image_h

#include "../core/element.h"
#include "pixel.h"

namespace laplace::graphics {
  class image : public core::element {
  public:
    using ref  = image &;
    using cref = const image &;
    using ptr  = std::shared_ptr<image>;

    image() = default;
    image(size_t width, size_t height, size_t depth = 1);
    ~image() = default;

    void clear();

    void set_size(size_t width, size_t height, size_t depth = 1);
    void set_size_radix2(size_t width, size_t height);
    void set_pixels(cref_vpixel pixels);

    auto mip(size_t level, size_t z = 0) const -> ptr;
    auto get_mip_count() const -> size_t;

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;
    auto get_depth() const -> size_t;
    auto get_plane() const -> size_t;

    auto get_size_bytes() const -> size_t;

    void set_pixel(size_t index, cref_pixel value);
    auto get_pixel(size_t index) const -> pixel;

    void set_pixel(size_t x, size_t y, cref_pixel value);
    auto get_pixel(size_t x, size_t y) const -> pixel;

    void set_pixel(size_t x, size_t y, size_t z, cref_pixel value);
    auto get_pixel(size_t x, size_t y, size_t z) const -> pixel;

    auto data() -> uint8_t *;
    auto get_data() const -> const uint8_t *;

  private:
    auto get_mip_size(size_t level) const -> size_t;
    auto get_average(size_t x0, size_t y0, size_t x1, size_t y1,
                     size_t z = 0) const -> pixel;

    size_t m_width     = 0;
    size_t m_height    = 0;
    size_t m_depth     = 0;
    size_t m_mip_size  = 0;
    size_t m_mip_count = 0;
    size_t m_plane     = 0;
    vpixel m_data;
  };

  using ref_image  = image::ref;
  using cref_image = image::cref;
  using ptr_image  = image::ptr;
}

#include "image.impl.h"

#endif
