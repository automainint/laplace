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
  static constexpr auto s_commands  = u8"_commands";
  static constexpr auto s_function  = u8"_function";
  static constexpr auto s_arguments = u8"_arguments";

  auto decode(fn_read read) -> pack_type;
  auto encode(fn_write write, const_pack_type data) -> bool;
}

#endif
