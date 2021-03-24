/*  laplace/ui/text/uit_lcd.cpp
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
#include "lcd.h"

namespace laplace::ui::text {
  using std::u8string_view, graphics::ref_image, graphics::cref_pixel;

  void lcd::set_size(    //
      int    char_top,   //
      size_t char_width, //
      size_t char_height) {

    m_char_top    = char_top;
    m_char_width  = char_width;
    m_char_height = char_height;

    m_pixels.resize(m_char_width * m_char_height * char_count, 0);
  }

  void lcd::set_scale(int x, int y) {
    m_scale_x = x;
    m_scale_y = y;
  }

  void lcd::set_spacing(int x, int y) {
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

  auto lcd::adjust(u8string_view text) -> painter::area {
    int      width = 0;
    char32_t c     = 0;

    for (size_t i = 0; utf8_decode(text, i, c);) {
      width += get_char_width(c);
    }

    return { get_char_top(), width, get_char_height() };
  }

  void lcd::print(ref_image img, int x, int y, u8string_view text) {
    char32_t code = 0;

    int x0 = x;
    int y0 = y - get_char_top();

    for (size_t i = 0, n = 0; utf8_decode(text, i, code); n++) {
      draw_char(img, x0, y0, code);

      x0 += static_cast<int>(get_char_width(code));
    }
  }

  void lcd::adjust_size(size_t index, char32_t c) {
    const auto x = get_char_x(c);
    const auto y = get_char_y(c);

    size_t i = 0;

    for (; i < m_char_width; i++) {
      bool is_empty = true;

      for (size_t j = 0; j < m_char_height; j++) {
        const auto n = get_pixel_index(x, y, i, j);

        if (n < m_pixels.size() && m_pixels[n]) {
          is_empty = false;
          break;
        }
      }

      if (!is_empty) {
        m_sizes[index].left = i;
        break;
      }
    }

    for (++i; i < m_char_width; i++) {
      bool is_empty = true;

      for (size_t j = 0; j < m_char_height; j++) {
        const auto n = get_pixel_index(x, y, i, j);

        if (n < m_pixels.size() && m_pixels[n]) {
          is_empty = false;
          break;
        }
      }

      if (is_empty) {
        m_sizes[index].size = static_cast<int>(i - m_sizes[index].left);
        break;
      }
    }

    if (m_sizes[index].size == 0) {
      m_sizes[index].size = static_cast<int>(m_char_width -
                                             m_sizes[index].left);
    }
  }

  auto lcd::get_char_x(char32_t c) const -> size_t {
    if (c < char_first || c >= char_first + char_count)
      c = char_first + char_count - 1;

    return ((c - char_first) % 16) * m_char_width;
  }

  auto lcd::get_char_y(char32_t c) const -> size_t {
    if (c < char_first || c >= char_first + char_count)
      c = char_first + char_count - 1;

    return ((c - char_first) / 16) * m_char_height;
  }

  auto lcd::get_pixel_index(size_t x, size_t y, size_t i,
                            size_t j) const -> size_t {
    return (y + m_char_height - j - 1) * (m_char_width * 16) + (x + i);
  }

  auto lcd::get_char_top() const -> int {
    return m_scale_y * m_char_top;
  }

  auto lcd::get_char_height() const -> int {
    return m_scale_y * static_cast<int>(m_char_height) + m_spacing_y;
  }

  auto lcd::get_char_left(char32_t c) const -> size_t {
    if (c < char_first || c >= char_first + char_count)
      return 0;

    return m_sizes[c - char_first].left;
  }

  auto lcd::get_char_size(char32_t c) const -> int {
    if (c < char_first || c >= char_first + char_count)
      return m_sizes[char_count - 1].size;

    return m_sizes[c - char_first].size;
  }

  auto lcd::get_char_width(char32_t c) const -> int {
    return get_char_size(c) * m_scale_x + m_spacing_x;
  }

  void lcd::draw_char(ref_image img, int x, int y, char32_t c) {
    const auto i0 = get_char_x(c) + get_char_left(c);
    const auto j0 = get_char_y(c);

    for (size_t j = 0; j < m_char_height; j++) {
      const auto size = get_char_size(c);

      for (size_t i = 0; i < static_cast<size_t>(size); i++) {
        size_t index = get_pixel_index(i0, j0, i, j);

        if (index < m_pixels.size() && m_pixels[index]) {
          auto x0 = static_cast<int>(i);
          auto y0 = static_cast<int>(j);

          auto cx = static_cast<int>(m_scale_x);
          auto cy = static_cast<int>(m_scale_y);

          draw_dot(img, x + x0 * cx, y + y0 * cy, x + (x0 + 1) * cx,
                   y + (y0 + 1) * cy, m_color);
        }
      }
    }
  }

  void lcd::draw_dot(ref_image img, int x0, int y0, int x1, int y1,
                     cref_pixel color) {
    for (int i = x0; i < x1; i++) {
      for (int j = y0; j < y1; j++) {
        img.set_pixel(              //
            static_cast<size_t>(i), //
            static_cast<size_t>(j), //
            color);
      }
    }
  }
}
