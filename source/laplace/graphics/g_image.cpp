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

  image::image(sl::whole width, sl::whole height, sl::whole depth) {
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

  void image::set_size(sl::whole width, sl::whole height,
                       sl::whole depth) {
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

  void image::set_size_radix2(sl::whole width, sl::whole height) {
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

  void image::set_pixel(sl::index n, cref_pixel value) {
    if (n >= 0 && n < m_data.size()) {
      m_data[n] = value;
    }
  }

  auto image::get_pixel(sl::index n) const -> pixel {
    if (n >= 0 && n < m_data.size()) {
      return m_data[n];
    }

    return {};
  }

  void image::set_pixel(sl::index x, sl::index y, cref_pixel value) {
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
      m_data[y * m_width + x] = value;
    }
  }

  auto image::get_pixel(sl::index x, sl::index y) const -> pixel {
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
      return m_data[y * m_width + x];
    }

    return {};
  }

  void image::set_pixel(sl::index x, sl::index y, sl::index z,
                        cref_pixel value) {
    if (x >= 0 && y >= 0 && z >= 0 && x < m_width && y < m_height &&
        z < m_depth) {
      m_data[z * m_plane + y * m_width + x] = value;
    }
  }

  auto image::get_pixel(sl::index x, sl::index y, sl::index z) const
      -> pixel {
    if (x >= 0 && y >= 0 && z >= 0 && x < m_width && y < m_height &&
        z < m_depth) {
      return m_data[z * m_plane + y * m_width + x];
    }

    return {};
  }

  auto image::get_mip_size(sl::index level) const -> size_t {
    auto size = m_mip_size;

    for (sl::index i = 0; i < level; i++) { size >>= 1; }

    return size;
  }

  auto image::get_average(sl::index x0, sl::index y0, sl::index x1,
                          sl::index y1, sl::index z) const -> pixel {
    auto value = pixel {};

    x0 = min(x0, m_width);
    y0 = min(y0, m_height);
    x1 = min(max(x0 + 1, x1), m_width);
    y1 = min(max(y0 + 1, y1), m_height);

    const auto count = (x1 - x0) * (y1 - y0);

    if (count > 0) {
      auto red   = double {};
      auto green = double {};
      auto blue  = double {};
      auto alpha = double {};

      for (sl::index y = y0; y < y1; y++) {
        for (sl::index x = x0; x < x1; x++) {
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

  auto image::mip(sl::index level, sl::index z) const -> image::ptr {
    auto result = make_shared<image>();

    const auto size  = get_mip_size(level);
    const auto fsize = static_cast<double>(size);

    result->set_size(size, size);

    for (sl::index j = 0; j < size; j++) {
      for (sl::index i = 0; i < size; i++) {
        auto x0 = static_cast<sl::index>(
            round(static_cast<double>(i * m_width) / fsize));
        auto y0 = static_cast<sl::index>(
            round(static_cast<double>(j * m_height) / fsize));
        auto x1 = static_cast<sl::index>(
            round(static_cast<double>((i + 1) * m_width) / fsize));
        auto y1 = static_cast<sl::index>(
            round(static_cast<double>((j + 1) * m_height) / fsize));

        result->set_pixel(i, j, get_average(x0, y0, x1, y1, z));
      }
    }

    return result;
  }
}
