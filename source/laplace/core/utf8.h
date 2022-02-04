/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_utf8_h
#define laplace_core_utf8_h

#include "defs.h"

namespace laplace::utf8 {
  [[nodiscard]] auto length(std::u8string_view bytes) noexcept
      -> sl::whole;
  [[nodiscard]] auto decode(std::u8string_view bytes,
                            sl::index         &offset,
                            char32_t          &code) noexcept -> bool;
  [[nodiscard]] auto encode(char32_t code, std::u8string &bytes,
                            sl::index &offset) noexcept -> bool;
}

#endif
