/*  laplace/ui/text/ttf.h
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

#ifndef laplace_ui_text_ttf_h
#define laplace_ui_text_ttf_h

#include "../../core/defs.h"
#include <cstdint>
#include <memory>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace laplace::ui::text {
  class ttf_library {
  public:
    ttf_library();
    ~ttf_library();

    auto new_face(const char *file_name, size_t index) -> FT_Face;
    auto new_memory_face(const uint8_t *bytes, size_t size,
                         size_t index) -> FT_Face;
    void done_face(FT_Face face);

  private:
    FT_Library m_library;
  };

  class ttf {
  public:
    /*  Global Library instance.
     */
    static std::weak_ptr<ttf_library> library;

    ttf();
    ~ttf();

    void open(std::string_view file_name);
    void open(std::wstring_view file_name);
    void load(cref_vbyte bytes);
    void done();

    void set_char_size(double width, double height);
    void set_pixel_sizes(size_t width, size_t height);
    auto load_char_render(char32_t c) -> bool;
    auto load_char_metrics(char32_t c) -> bool;
    auto get_glyph() -> FT_GlyphSlot;

  private:
    std::shared_ptr<ttf_library> m_lib;
    FT_Face                      m_face;
  };
}

#endif
