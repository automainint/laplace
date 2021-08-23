/*  laplace/ui/text/buffer.h
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

#ifndef laplace_ui_text_buffer_h
#define laplace_ui_text_buffer_h

#include "../../graphics/texture.h"
#include "painter.h"
#include "renderer.h"
#include <array>

namespace laplace::ui::text {
  /*  Buffered 16x16 font.
   */
  class buffer final : public renderer {
  public:
    static constexpr size_t char_count = 256;

    ~buffer() final = default;

    void build(ref_painter paint);
    void build_monospace(ref_painter paint, sl::index top,
                         sl::index char_width, sl::index char_height);
    void build_monospace(graphics::cref_image img, sl::index top,
                         sl::index char_width, sl::index char_height);

    auto adjust(std::u8string_view text) -> area final;
    void render(sl::index x, sl::index y, std::u8string_view text) final;

  private:
    int    m_cell_top    = 0;
    size_t m_cell_width  = 0;
    size_t m_cell_height = 0;

    std::array<area, char_count> m_chars;
    graphics::ptr_texture        m_texture =
        std::make_shared<graphics::texture>();
  };
}

#endif
