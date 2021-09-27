/*  laplace/ui/elem/uie_textarea.cpp
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

#include "textarea.h"

namespace laplace::ui::elem {
  using std::u8string_view;

  const sl::whole textarea::default_line_height = 30;

  void textarea::render(context const &con) {
    textarea_render(con);
    up_to_date();
  }

  void textarea::set_text(u8string_view text) {
    m_text = text;
    set_expired(true);
  }

  void textarea::set_line_height(int line_height) {
    m_line_height = line_height;
    set_expired(true);
  }

  void textarea::textarea_render(context const &con) {
    auto const r  = get_rect();
    auto const y1 = get_absolute_y() + r.height;
    auto       y  = m_line_height > 0 ? get_absolute_y()
                                      : y1 - m_line_height;

    auto i0 = sl::index {};

    for (sl::index i = 0; i <= m_text.length(); i++) {
      if (i < m_text.length() && m_text[i] != u8'\n')
        continue;
      if (i0 < i)
        con.render_text({ .x      = get_absolute_x(),
                           .y      = y,
                           .width  = r.width,
                           .height = y1 - y },
                         { m_text.begin() + i0, m_text.begin() + i });
      if (y < 0)
        break;
      y += m_line_height;
      if (y >= y1)
        break;
      i0 = i + 1;
    }
  }
}
