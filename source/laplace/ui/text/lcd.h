#pragma once

#include "../../core/defs.h"
#include "painter.h"

namespace laplace::ui::text {
  /*  LCD bit text renderer.
   */
  class lcd final : public painter {
  public:
    static constexpr sl::whole char_first = 32;
    static constexpr sl::whole char_count = 96;

    static constexpr sl::index default_scale_x   = 3;
    static constexpr sl::index default_scale_y   = 3;
    static constexpr sl::index default_spacing_x = 4;
    static constexpr sl::index default_spacing_y = 4;

    ~lcd() override = default;

    void set_size(sl::index char_top, sl::whole char_width,
                  sl::whole char_height);
    void set_scale(sl::index x, sl::index y);
    void set_spacing(sl::index x, sl::index y);

    void set_bits(const uint64_t *bits);

    auto adjust(std::u8string_view text) -> area final;

    void print(graphics::ref_image img, sl::index x, sl::index y,
               std::u8string_view text) final;

  private:
    void adjust_size(sl::index n, char32_t c);

    auto get_char_x(char32_t c) const -> sl::whole;
    auto get_char_y(char32_t c) const -> sl::whole;

    auto get_pixel_index(sl::index x, sl::index y, sl::index i,
                         sl::index j) const -> sl::index;

    auto get_char_top() const -> sl::index;
    auto get_char_height() const -> sl::whole;

    auto get_char_left(char32_t c) const -> sl::whole;
    auto get_char_size(char32_t c) const -> sl::index;
    auto get_char_width(char32_t c) const -> sl::index;

    void draw_char(graphics::ref_image img, sl::index x, sl::index y,
                   char32_t c);
    static void draw_dot(graphics::ref_image img, sl::index x0,
                         sl::index y0, sl::index x1, sl::index y1,
                         graphics::cref_pixel color);

    struct char_size {
      sl::whole left = 0;
      sl::index size = 0;
    };

    char_size m_sizes[char_count];

    graphics::pixel m_color = default_color;

    sl::index m_scale_x     = default_scale_x;
    sl::index m_scale_y     = default_scale_y;
    sl::index m_spacing_x   = default_spacing_x;
    sl::index m_spacing_y   = default_spacing_y;
    sl::index m_char_top    = 0;
    sl::whole m_char_width  = 0;
    sl::whole m_char_height = 0;
    vbyte     m_pixels;
  };
}
