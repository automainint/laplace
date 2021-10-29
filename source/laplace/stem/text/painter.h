/*  laplace/stem/text/painter.h
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

#ifndef laplace_stem_text_painter_h
#define laplace_stem_text_painter_h

#include "../../graphics/image.h"
#include "../../ui/context.h"
#include <string>

namespace laplace::stem::text {
  /*  Rendering text into image.
   */
  class painter {
  public:
    using ptr_painter = std::shared_ptr<painter>;

    static sl::index const default_lcd_char_top;
    static sl::index const default_lcd_char_width;
    static sl::index const default_lcd_char_height;
    static uint64_t const  default_lcd_bits[];

    static constexpr auto default_color = graphics::rgba(
        250, 240, 255, 255);

    virtual ~painter() = default;

    virtual auto adjust(std::u8string_view text) -> ui::text_area;

    virtual void print(graphics::ref_image img,
                       sl::index           x,
                       sl::index           y,
                       std::u8string_view  text);

    static auto get_default() -> ptr_painter;

  private:
    static std::weak_ptr<painter> m_default;
  };

  using ref_painter  = painter &;
  using cref_painter = const painter &;
  using ptr_painter  = painter::ptr_painter;
}

#endif
