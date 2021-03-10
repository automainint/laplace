/*  laplace/ui/text/uit_font.cpp
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

#include "../../core/utils.h"
#include "font.h"
#include <algorithm>

namespace laplace::ui::text {
  using std::u8string_view, std::min, std::max, graphics::ref_image,
      graphics::cref_pixel, graphics::pixel, graphics::rgba;

  void font::load(u8string_view file_name) {
    m_face.open(to_wstring(file_name));
  }

  void font::set_size(size_t width, size_t height) {
    m_face.set_pixel_sizes(width, height);
  }

  auto font::adjust(u8string_view text) -> painter::area {
    int top         = 0;
    int width       = 0;
    int width_total = 0;
    int height      = 0;

    char32_t code = 0;

    for (size_t i = 0; utf8_decode(text, i, code);) {
      if (m_face.load_char_metrics(code)) {
        auto slot = m_face.get_glyph();

        if (top < slot->bitmap_top) {
          height += slot->bitmap_top - top;
          top = slot->bitmap_top;
        }

        if (height < static_cast<int>(slot->bitmap.rows)) {
          height = static_cast<int>(slot->bitmap.rows);
        }

        width_total = width + slot->bitmap.width;
        width += slot->advance.x >> 6;
      }
    }

    top    = max(0, top);
    width  = max(0, width_total);
    height = max(0, height);

    return { top, width, height };
  }

  void font::print(ref_image img, int x, int y, u8string_view text) {
    char32_t code = 0;

    for (size_t i = 0; utf8_decode(text, i, code);) {
      if (m_face.load_char_render(code)) {
        auto slot = m_face.get_glyph();

        draw(img, x + slot->bitmap_left, y - slot->bitmap_top,
             slot->bitmap, m_color);

        x += slot->advance.x >> 6;
      }
    }
  }

  void font::draw(ref_image img, int x0, int y0, cref_bitmap bitmap,
                  cref_pixel color) {
    for (size_t y = 0; y < bitmap.rows; y++) {
      size_t row = bitmap.pitch > 0 ? y : (bitmap.rows - (y + 1));

      auto p = static_cast<const uint8_t *>(bitmap.buffer +
                                            row * abs(bitmap.pitch));

      for (size_t x = 0; x < bitmap.width; x++) {
        auto i = x0 + x;
        auto j = y0 + y;
        auto s = img.get_pixel(i, j);
        auto c = blend(s, color, p[x]);
        img.set_pixel(i, j, c);
      }
    }
  }

  auto font::blend(cref_pixel s, cref_pixel d, uint8_t factor)
      -> pixel {
    auto mul = [](uint8_t a, uint8_t b) -> uint8_t {
      return static_cast<uint8_t>(
          min(255, (static_cast<int>(a) * b + 127) / 255));
    };

    auto alpha = mul(d.alpha, factor);

    return alpha == 0
               ? s
               : rgba(d.red, d.green, d.blue, max(s.alpha, alpha));
  }

  auto font::get_color() const -> cref_pixel {
    return m_color;
  }
}
