/*  laplace/format/text.h
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

#ifndef laplace_format_text_h
#define laplace_format_text_h

#include "defs.h"

namespace laplace::format::text {
  static constexpr auto s_commands  = u8"__commands";
  static constexpr auto s_function  = u8"__function";
  static constexpr auto s_arguments = u8"__arguments";

  [[nodiscard]] auto decode(fn_read read) noexcept -> pack_type;
  [[nodiscard]] auto encode(fn_write        write,
                            const_pack_type data) noexcept -> bool;
}

#endif
