/*  laplace/ui/text/uit_wrap.cpp
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

#include "wrap.h"

namespace laplace::ui::text {
  using std::u8string_view, graphics::image;

  wrap::wrap(ptr_painter paint, ptr_context cont) {
    m_context = cont;
    m_paint   = paint;
  }

  void wrap::set_context(ptr_context cont) {
    m_context = cont;
  }

  auto wrap::adjust(u8string_view text) -> area {
    return m_paint ? m_paint->adjust(text) : area {};
  }

  void wrap::render(sl::index x, sl::index y, u8string_view text) {
    if (m_paint && m_tex) {
      auto img = image {};
      auto a   = m_paint->adjust(text);

      img.set_size(a.width, a.height);

      m_paint->print(img, 0, a.top, text);
      m_tex->image_2d_nearest(img);

      if (!m_context) {
        m_context = context::get_default();
      }

      m_context->render(
          rect { .x = x, .y = y, .width = a.width, .height = a.height },
          *m_tex);
    }
  }
}
