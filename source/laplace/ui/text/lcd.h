#pragma once

#include "../../core/defs.h"
#include "painter.h"

namespace laplace::ui::text {
  /*  LCD bit text renderer.
   */
  class lcd final : public painter {
  public:
    static constexpr size_t char_first = 32;
    static constexpr size_t char_count = 96;

    static constexpr int default_scale_x   = 3;
    static constexpr int default_scale_y   = 3;
    static constexpr int default_spacing_x = 4;
    static constexpr int default_spacing_y = 4;

    ~lcd() override = default;

    void set_size(int char_top, size_t char_width, size_t char_height);
    void set_scale(int x, int y);
    void set_spacing(int x, int y);

    void set_bits(const uint64_t *bits);

    auto adjust(std::u8string_view text) -> area final;

    void print(                  //
        graphics::ref_image img, //
        int x, int y,            //
        std::u8string_view text) final;

  private:
    void adjust_size(size_t index, char32_t c);

    auto get_char_x(char32_t c) const -> size_t;
    auto get_char_y(char32_t c) const -> size_t;

    auto get_pixel_index(size_t x, size_t y, size_t i, size_t j) const
        -> size_t;

    auto get_char_top() const -> int;
    auto get_char_height() const -> int;

    auto get_char_left(char32_t c) const -> size_t;
    auto get_char_size(char32_t c) const -> int;
    auto get_char_width(char32_t c) const -> int;

    void draw_char(graphics::ref_image img, int x, int y, char32_t c);
    static void draw_dot(graphics::ref_image img, int x0, int y0,
                         int x1, int y1, graphics::cref_pixel color);

    struct char_size {
      size_t left = 0;
      int    size = 0;
    };

    char_size m_sizes[char_count];

    graphics::pixel m_color = default_color;

    int    m_scale_x     = default_scale_x;
    int    m_scale_y     = default_scale_y;
    int    m_spacing_x   = default_spacing_x;
    int    m_spacing_y   = default_spacing_y;
    int    m_char_top    = 0;
    size_t m_char_width  = 0;
    size_t m_char_height = 0;
    vbyte  m_pixels;
  };
}
