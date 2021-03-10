/*  laplace/graphics/texture.h
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

#ifndef laplace_graphics_texture_h
#define laplace_graphics_texture_h

#include "image.h"

namespace laplace::graphics {
  class texture : public core::element {
  public:
    enum wrap { clamp, repeat, mirror };

    texture();
    ~texture() override;

    texture(texture &&tex) noexcept;
    auto operator=(texture &&tex) noexcept -> texture &;

    void bind_2d(size_t index);
    void bind_3d(size_t index);

    void set_wrap_x(wrap mode);
    void set_wrap_y(wrap mode);
    void set_wrap_z(wrap mode);

    /*  Linear magnification and minifying
     *  functions.
     */
    void image_2d_linear(cref_image img);

    /*  Nearest magnification function and
     *  linear minifying function.
     */
    void image_2d_nearest(cref_image img);

    /*  Linear magnification function and
     *  linear mipmaps for minifying.
     */
    void mipmaps_2d_linear(cref_image img);

    /*  Nearest magnification function and
     *  linear mipmaps for minifying.
     */
    void mipmaps_2d_nearest(cref_image img);

    void image_3d_linear(cref_image img);
    void image_3d_nearest(cref_image img);

    auto get_id() const -> uint32_t;

    texture(texture &) = delete;
    auto operator=(texture &) -> texture & = delete;

  private:
    bool     m_ok;
    int32_t  m_wrap[3];
    uint32_t m_id;
  };

  using ref_texture = texture &;
  using ptr_texture = std::shared_ptr<texture>;
}

#endif
