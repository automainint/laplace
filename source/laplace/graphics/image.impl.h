/*  laplace/graphics/image.impl.h
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

#ifndef laplace_graphics_image_impl_h
#define laplace_graphics_image_impl_h

namespace laplace::graphics {
  inline auto image::get_mip_count() const -> size_t {
    return this->m_mip_count;
  }

  inline auto image::get_width() const -> size_t {
    return this->m_width;
  }

  inline auto image::get_height() const -> size_t {
    return this->m_height;
  }

  inline auto image::get_depth() const -> size_t {
    return this->m_depth;
  }

  inline auto image::get_plane() const -> size_t {
    return this->m_plane;
  }

  inline auto image::get_size_bytes() const -> size_t {
    return m_width * m_height * m_depth * sizeof(pixel);
  }

  inline auto image::data() -> uint8_t * {
    return reinterpret_cast<uint8_t *>(this->m_data.data());
  }

  inline auto image::get_data() const -> const uint8_t * {
    return reinterpret_cast<const uint8_t *>(this->m_data.data());
  }
}

#endif
