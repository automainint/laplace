/*  laplace/ui/elem/uie_canvas.cpp
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

#include "canvas.h"

#include "../context.h"

namespace laplace::ui::elem {
  using graphics::ref_image, graphics::cref_image;

  canvas::canvas() {
    m_context = context::get_default();
  }

  void canvas::set_context(ptr_context cont) {
    m_context = cont ? cont : context::get_default();
    set_expired(true);
  }

  void canvas::render() {
    m_texture.image_2d_nearest(m_image);
    m_context->render(get_rect(), m_texture);
  }

  auto canvas::image() -> ref_image {
    return m_image;
  }

  auto canvas::get_image() const -> cref_image {
    return m_image;
  }
}
