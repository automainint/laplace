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

#include "../core/defs.h"
#include "pixel.h"

namespace laplace::graphics {
  class image {
  public:
    using ref  = image &;
    using cref = const image &;
    using ptr  = std::shared_ptr<image>;

    image() = default;
    image(sl::whole width, sl::whole height, sl::whole depth = 1);
    ~image() = default;

    void clear();

    void set_size(sl::whole width, sl::whole height,
                  sl::whole depth = 1);
    void set_size_radix2(sl::whole width, sl::whole height);
    void set_pixels(cref_vpixel pixels);

    auto mip(sl::index level, sl::index z = 0) const -> ptr;
    auto get_mip_count() const -> sl::whole;

    auto get_width() const -> sl::whole;
    auto get_height() const -> sl::whole;
    auto get_depth() const -> sl::whole;
    auto get_plane() const -> sl::whole;

    auto get_size_bytes() const -> sl::whole;

    void set_pixel(sl::index n, cref_pixel value);
    auto get_pixel(sl::index n) const -> pixel;

    void set_pixel(sl::index x, sl::index y, cref_pixel value);
    auto get_pixel(sl::index x, sl::index y) const -> pixel;

    void set_pixel(sl::index x, sl::index y, sl::index z,
                   cref_pixel value);
    auto get_pixel(sl::index x, sl::index y, sl::index z) const
        -> pixel;

    auto data() -> uint8_t *;
    auto get_data() const -> const uint8_t *;

  private:
    auto get_mip_size(sl::index level) const -> size_t;
    auto get_average(sl::index x0, sl::index y0, sl::index x1,
                     sl::index y1, sl::index z = 0) const -> pixel;

    sl::whole m_width     = 0;
    sl::whole m_height    = 0;
    sl::whole m_depth     = 0;
    sl::whole m_mip_size  = 0;
    sl::whole m_mip_count = 0;
    sl::whole m_plane     = 0;
    vpixel    m_data;
  };

  using ref_image  = image::ref;
  using cref_image = image::cref;
  using ptr_image  = image::ptr;
}

#include "image.impl.h"

#endif
