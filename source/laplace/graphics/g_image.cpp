/*  laplace/graphics/g_image.cpp
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

#include "image.h"
#include <algorithm>

namespace laplace::graphics {
  using std::make_shared, std::min, std::max, std::copy;

  image::image(size_t width, size_t height, size_t depth) {
    set_size(width, height, depth);
  }

  void image::clear() {
    m_width     = 0;
    m_height    = 0;
    m_depth     = 0;
    m_mip_size  = 0;
    m_mip_count = 0;
    m_plane     = 0;
    m_data.clear();
  }

  void image::set_size(size_t width, size_t height, size_t depth) {
    m_width  = width;
    m_height = height;
    m_depth  = depth;

    m_mip_count = 1;
    m_mip_size  = 1;

    while (m_mip_size < m_width || m_mip_size < m_height) {
      m_mip_size <<= 1;
      m_mip_count++;
    }

    m_plane = m_width * m_height;
    m_data.resize(m_plane * m_depth);

    if (m_data.empty()) {
      m_width     = 0;
      m_height    = 0;
      m_depth     = 0;
      m_mip_size  = 0;
      m_mip_count = 0;
      m_plane     = 0;
    }
  }

  void image::set_size_radix2(size_t width, size_t height) {
    m_mip_count = 1;
    m_mip_size  = 1;

    while (m_mip_size < width || m_mip_size < height) {
      m_mip_size <<= 1;
      m_mip_count++;
    }

    m_width  = m_mip_size;
    m_height = m_mip_size;
    m_depth  = 1;

    m_plane = m_width * m_height;
    m_data.resize(m_plane * m_depth);

    if (m_data.empty()) {
      clear();
    }
  }

  void image::set_pixels(cref_vpixel pixels) {
    auto size = max(pixels.size(), m_data.size());

    copy(pixels.begin(), pixels.begin() + static_cast<ptrdiff_t>(size),
         m_data.begin());
  }

  void image::set_pixel(size_t index, cref_pixel value) {
    if (index < m_data.size()) {
      m_data[index] = value;
    }
  }

  auto image::get_pixel(size_t index) const -> pixel {
    pixel p;

    if (index < m_data.size()) {
      p = m_data[index];
    }

    return p;
  }

  void image::set_pixel(size_t x, size_t y, cref_pixel value) {
    if (x < m_width && y < m_height) {
      m_data[y * m_width + x] = value;
    }
  }

  auto image::get_pixel(size_t x, size_t y) const -> pixel {
    pixel p;

    if (x < m_width && y < m_height) {
      p = m_data[y * m_width + x];
    }

    return p;
  }

  void image::set_pixel(size_t x, size_t y, size_t z, cref_pixel value) {
    if (x < m_width && y < m_height && z < m_depth) {
      m_data[z * m_plane + y * m_width + x] = value;
    }
  }

  auto image::get_pixel(size_t x, size_t y, size_t z) const -> pixel {
    pixel p;

    if (x < m_width && y < m_height) {
      p = m_data[z * m_plane + y * m_width + x];
    }

    return p;
  }

  auto image::get_mip_size(size_t level) const -> size_t {
    auto size = m_mip_size;

    for (size_t i = 0; i < level; i++) { size >>= 1; }

    return size;
  }

  auto image::get_average(size_t x0, size_t y0, size_t x1, size_t y1,
                          size_t z) const -> pixel {
    pixel value;

    x0 = min(x0, m_width);
    y0 = min(y0, m_height);
    x1 = min(max(x0 + 1, x1), m_width);
    y1 = min(max(y0 + 1, y1), m_height);

    const auto count = (x1 - x0) * (y1 - y0);

    if (count > 0) {
      double red   = 0;
      double green = 0;
      double blue  = 0;
      double alpha = 0;

      for (size_t y = y0; y < y1; y++) {
        for (size_t x = x0; x < x1; x++) {
          auto p = get_pixel(x, y, z);

          red += static_cast<double>(p.red);
          green += static_cast<double>(p.green);
          blue += static_cast<double>(p.blue);
          alpha += static_cast<double>(p.alpha);
        }
      }

      red /= static_cast<double>(count);
      green /= static_cast<double>(count);
      blue /= static_cast<double>(count);
      alpha /= static_cast<double>(count);

      value.red = static_cast<uint8_t>(
          max(0, min(static_cast<int>(red), 255)));
      value.green = static_cast<uint8_t>(
          max(0, min(static_cast<int>(green), 255)));
      value.blue = static_cast<uint8_t>(
          max(0, min(static_cast<int>(blue), 255)));
      value.alpha = static_cast<uint8_t>(
          max(0, min(static_cast<int>(alpha), 255)));
    }

    return value;
  }

  auto image::mip(size_t level, size_t z) const -> image::ptr {
    ptr result = make_shared<image>();

    const auto size  = get_mip_size(level);
    const auto fsize = static_cast<double>(size);

    result->set_size(size, size);

    for (size_t j = 0; j < size; j++) {
      for (size_t i = 0; i < size; i++) {
        auto x0 = static_cast<size_t>(
            static_cast<double>(i * m_width) / fsize);
        auto y0 = static_cast<size_t>(
            static_cast<double>(j * m_height) / fsize);
        auto x1 = static_cast<size_t>(
            static_cast<double>((i + 1) * m_width) / fsize);
        auto y1 = static_cast<size_t>(
            static_cast<double>((j + 1) * m_height) / fsize);

        result->set_pixel(i, j, get_average(x0, y0, x1, y1, z));
      }
    }

    return result;
  }
}
