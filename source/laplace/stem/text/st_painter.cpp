/*  laplace/stem/text/st_painter.cpp
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

#include "painter.h"

#include "lcd.h"
#include <iostream>

namespace laplace::stem::text {
  using std::make_shared, std::weak_ptr, std::u8string_view;

  weak_ptr<painter> painter::m_default;

  auto painter::adjust(u8string_view text) -> ui::text_area {
    return { 0, 0, 0 };
  }

  void painter::print(graphics::ref_image img, sl::index x,
                      sl::index y, u8string_view text) { }

  auto painter::get_default(log_handler log) -> ptr_painter {
    auto p = m_default.lock();

    if (!p) {
      log(log_event::verbose, "Init default LCD font.",
          "Stem/Text/Painter");

      auto lcd_font = make_shared<lcd>();

      lcd_font->set_size(default_lcd_char_top, default_lcd_char_width,
                         default_lcd_char_height);

      lcd_font->set_bits(default_lcd_bits);

      p         = lcd_font;
      m_default = p;
    }

    return p;
  }
}
