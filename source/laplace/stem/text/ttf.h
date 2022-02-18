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

#ifndef laplace_stem_text_ttf_h
#define laplace_stem_text_ttf_h

#include "../../core/defs.h"
#include <cstdint>
#include <memory>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace laplace::stem::text {
  class ttf_library {
  public:
    log_handler log = get_global_log();

    ttf_library();
    ~ttf_library();

    [[nodiscard]] auto new_face(const char *file_name, sl::index n)
        -> FT_Face;
    [[nodiscard]] auto new_memory_face(const uint8_t *bytes,
                                       sl::whole size, sl::index n)
        -> FT_Face;
    void done_face(FT_Face face);

  private:
    FT_Library m_library;
  };

  class ttf {
  public:
    /*  Global Library instance.
     */
    static std::weak_ptr<ttf_library> library;

    log_handler log = get_global_log();

    ttf();
    ~ttf();

    void open(std::string_view file_name);
    void open(std::wstring_view file_name);
    void load(span_cbyte bytes);
    void done();

    void set_char_size(double width, double height);
    void set_pixel_sizes(sl::whole width, sl::whole height);

    [[nodiscard]] auto load_char_render(char32_t c) -> bool;
    [[nodiscard]] auto load_char_metrics(char32_t c) -> bool;
    [[nodiscard]] auto get_glyph() -> FT_GlyphSlot;

  private:
    std::shared_ptr<ttf_library> m_lib;
    FT_Face                      m_face;
  };
}

#endif
