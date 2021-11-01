/*  laplace/stem/text/st_lcd.cpp
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

#include "lcd.h"

#include "../../core/utils.h"

namespace laplace::stem::text {
  using std::u8string_view, graphics::ref_image, graphics::cref_pixel;

  void lcd::set_size(sl::index char_top,
                     sl::whole char_width,
                     sl::whole char_height) {

    m_char_top    = char_top;
    m_char_width  = char_width;
    m_char_height = char_height;

    m_pixels.resize(m_char_width * m_char_height * char_count, 0);
  }

  void lcd::set_scale(sl::index x, sl::index y) {
    m_scale_x = x;
    m_scale_y = y;
  }

  void lcd::set_spacing(sl::index x, sl::index y) {
    m_spacing_x = x;
    m_spacing_y = y;
  }

  void lcd::set_bits(const uint64_t *bits) {
    size_t i = 0, k = 0;

    size_t pixel_count = m_char_width * m_char_height * char_count;

    if (pixel_count > m_pixels.size()) {
      pixel_count = m_pixels.size();
    }

    for (size_t index = 0; index < pixel_count; index++) {
      m_pixels[index] = (bits[i] & (1ull << k)) ? 0u : 1u;

      if (++k >= 64) {
        i++;
        k = 0;
      }
    }

    for (i = 0; i < char_count; i++) {
      adjust_size(i, static_cast<char32_t>(char_first + i));
    }
  }

  auto lcd::adjust(u8string_view text) -> ui::text_area {
    auto width = sl::index {};
    auto c     = char32_t {};

    for (sl::index i = 0; utf8::decode(text, i, c);) {
      width += get_char_width(c);
    }

    return { get_char_top(), width, get_char_height() };
  }

  void lcd::print(ref_image     img,
                  sl::index     x,
                  sl::index     y,
                  u8string_view text) {
    auto code = char32_t {};

    auto x0 = x;
    auto y0 = y - get_char_top();

    for (sl::index i = 0, n = 0; utf8::decode(text, i, code); n++) {
      draw_char(img, x0, y0, code);

      x0 += static_cast<int>(get_char_width(code));
    }
  }

  void lcd::adjust_size(sl::index n, char32_t c) {
    auto const x = get_char_x(c);
    auto const y = get_char_y(c);

    auto const is_line_empty = [&](sl::index i) -> bool {
      for (sl::index j = 0; j < m_char_height; j++) {
        const auto k = get_pixel_index(x, y, i, j);
        if (k < m_pixels.size() && m_pixels[k])
          return false;
      }

      return true;
    };

    for (sl::index i = 0; i < m_char_width; i++)
      if (!is_line_empty(i)) {
        m_sizes[n].left = i;
        break;
      }

    for (sl::index i = m_char_width - 1; i > 0; i--)
      if (!is_line_empty(i)) {
        m_sizes[n].size = i - m_sizes[n].left + 1;
        break;
      }
    
    if (m_sizes[n].size == 0) {
      m_sizes[n].size = m_char_width - m_sizes[n].left;
    }
  }

  auto lcd::get_char_x(char32_t c) const -> sl::whole {
    if (c < char_first || c >= char_first + char_count)
      c = char_first + char_count - 1;

    return ((c - char_first) % 16) * m_char_width;
  }

  auto lcd::get_char_y(char32_t c) const -> sl::whole {
    if (c < char_first || c >= char_first + char_count)
      c = char_first + char_count - 1;

    return ((c - char_first) / 16) * m_char_height;
  }

  auto lcd::get_pixel_index(sl::index x,
                            sl::index y,
                            sl::index i,
                            sl::index j) const -> sl::index {
    return (y + m_char_height - j - 1) * (m_char_width * 16) +
           (x + i);
  }

  auto lcd::get_char_top() const -> sl::index {
    return m_scale_y * m_char_top;
  }

  auto lcd::get_char_height() const -> sl::index {
    return m_scale_y * static_cast<int>(m_char_height) + m_spacing_y;
  }

  auto lcd::get_char_left(char32_t c) const -> sl::whole {
    if (c < char_first || c >= char_first + char_count)
      return 0;

    return m_sizes[c - char_first].left;
  }

  auto lcd::get_char_size(char32_t c) const -> sl::index {
    if (c < char_first || c >= char_first + char_count)
      return m_sizes[char_count - 1].size;

    return m_sizes[c - char_first].size;
  }

  auto lcd::get_char_width(char32_t c) const -> sl::index {
    return get_char_size(c) * m_scale_x + m_spacing_x;
  }

  void lcd::draw_char(ref_image img,
                      sl::index x,
                      sl::index y,
                      char32_t  c) {
    const auto i0 = get_char_x(c) + get_char_left(c);
    const auto j0 = get_char_y(c);

    for (sl::index j = 0; j < m_char_height; j++) {
      const auto size = get_char_size(c);

      for (sl::index i = 0; i < size; i++) {
        const auto n = get_pixel_index(i0, j0, i, j);

        if (n < m_pixels.size() && m_pixels[n]) {
          const auto cx = m_scale_x;
          const auto cy = m_scale_y;

          draw_dot(img, x + i * cx, y + j * cy, x + (i + 1) * cx,
                   y + (j + 1) * cy, m_color);
        }
      }
    }
  }

  void lcd::draw_dot(ref_image  img,
                     sl::index  x0,
                     sl::index  y0,
                     sl::index  x1,
                     sl::index  y1,
                     cref_pixel color) {
    for (sl::index i = x0; i < x1; i++)
      for (sl::index j = y0; j < y1; j++) {
        img.set_pixel(i, j, color);
      }
  }
}
