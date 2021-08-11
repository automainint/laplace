/*  laplace/ui/text/uit_ttf.cpp
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

#include "ttf.h"

#include "../../core/utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace laplace::ui::text {
  namespace fs = std::filesystem;

  using std::make_shared, std::string, std::string_view,
      std::wstring_view, std::weak_ptr, std::ifstream, std::ios;

  ttf_library::ttf_library() {
    auto e = FT_Init_FreeType(&m_library);

    if (e) {
      error_("FT_Init_FreeType failed.", __FUNCTION__);
      m_library = nullptr;
    }
  }

  ttf_library::~ttf_library() {
    if (m_library) {
      FT_Done_FreeType(m_library);
    }
  }

  auto ttf_library::new_face(const char *file_name, sl::index n)
      -> FT_Face {
    FT_Face face = nullptr;

    if (m_library) {
      auto e = FT_New_Face(m_library, file_name,
                           static_cast<FT_Long>(n), &face);

      if (e) {
        error_("FT_New_Face failed.", __FUNCTION__);
        face = nullptr;
      }
    }

    return face;
  }

  auto ttf_library::new_memory_face(const uint8_t *bytes,
                                    sl::whole size, sl::index n)
      -> FT_Face {
    FT_Face face = nullptr;

    if (m_library) {
      auto e = FT_New_Memory_Face(m_library, bytes,
                                  static_cast<FT_Long>(size),
                                  static_cast<FT_Long>(n), &face);

      if (e) {
        error_("FT_New_Memory_Face failed.", __FUNCTION__);
        face = nullptr;
      }
    }

    return face;
  }

  void ttf_library::done_face(FT_Face face) {
    if (m_library) {
      FT_Done_Face(face);
    }
  }

  weak_ptr<ttf_library> ttf::library;

  ttf::ttf() {
    if (m_lib = library.lock(); !m_lib) {
      m_lib   = make_shared<ttf_library>();
      library = m_lib;
    }

    m_face = nullptr;
  }

  ttf::~ttf() {
    done();
  }

  void ttf::open(string_view file_name) {
    done();

    m_face = m_lib->new_face(file_name.data(), 0);
  }

  void ttf::open(wstring_view file_name) {
    auto asci    = string(file_name.size(), 0);
    auto is_asci = true;

    for (size_t i = 0; i < file_name.size(); i++) {
      if (file_name[i] > 0xFF) {
        is_asci = false;
        break;
      }

      asci[i] = static_cast<char>(file_name[i]);
    }

    if (is_asci) {
      open(asci);

    } else {
      auto in = ifstream(fs::path(file_name));

      if (!in) {
        error_("Can't open file.", __FUNCTION__);
        return;
      }

      auto buffer = vbyte {};

      in.seekg(ios::end);
      buffer.resize(as_index(in.tellg()));
      in.seekg(ios::beg);
      in.read(reinterpret_cast<char *>(buffer.data()),
              static_cast<ptrdiff_t>(buffer.size()));
      buffer.resize(as_index(in.tellg()));

      load(buffer);
    }
  }

  void ttf::load(span_cbyte bytes) {
    done();

    m_face = m_lib->new_memory_face(bytes.data(), bytes.size(), 0);
  }

  void ttf::done() {
    if (m_face) {
      m_lib->done_face(m_face);
      m_face = nullptr;
    }
  }

  void ttf::set_char_size(double width, double height) {
    if (m_face) {
      auto e = FT_Set_Char_Size(
          m_face, static_cast<FT_F26Dot6>(width * 64),
          static_cast<FT_F26Dot6>(height * 64), 300, 300);

      if (e) {
        error_("FT_Set_Char_Size failed.", __FUNCTION__);
      }
    }
  }

  void ttf::set_pixel_sizes(sl::whole width, sl::whole height) {
    if (m_face) {
      auto e = FT_Set_Pixel_Sizes(m_face, static_cast<FT_UInt>(width),
                                  static_cast<FT_UInt>(height));

      if (e) {
        error_("FT_Set_Pixel_Sizes failed.", __FUNCTION__);
      }
    }
  }

  auto ttf::load_char_render(char32_t c) -> bool {
    bool result = false;

    if (m_face) {
      result = FT_Load_Char(m_face, c, FT_LOAD_RENDER) ? false : true;
    }

    return result;
  }

  auto ttf::load_char_metrics(char32_t c) -> bool {
    bool result = false;

    if (m_face) {
      result = FT_Load_Char(m_face, c, FT_LOAD_BITMAP_METRICS_ONLY)
                   ? false
                   : true;
    }

    return result;
  }

  auto ttf::get_glyph() -> FT_GlyphSlot {
    return m_face ? m_face->glyph : nullptr;
  }
}
