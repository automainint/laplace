/*  laplace/ui/text/font.h
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

#ifndef laplace_ui_text_font_h
#define laplace_ui_text_font_h

#include "painter.h"
#include "ttf.h"
#include <memory>

namespace laplace::ui::text {
  /*  TTF font renderer.
   */
  class font final : public painter {
  public:
    using cref_bitmap = const FT_Bitmap &;

    ~font() final = default;

    void load(std::u8string_view file_name);
    void set_size(size_t width, size_t height);

    auto adjust(std::u8string_view text) -> area final;
    void print(graphics::ref_image img, sl::index x, sl::index y,
               std::u8string_view text) final;

    auto get_color() const -> graphics::cref_pixel;

  private:
    static void draw(graphics::ref_image img, sl::index x0,
                     sl::index y0, cref_bitmap bitmap,
                     graphics::cref_pixel color);
    static auto blend(graphics::cref_pixel s, graphics::cref_pixel d,
                      uint8_t factor) -> graphics::pixel;

    graphics::pixel m_color = default_color;
    ttf             m_face;
  };

  using ref_font  = font &;
  using cref_font = const font &;
  using ptr_font  = std::shared_ptr<font>;
}

#endif
