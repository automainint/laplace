/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_format_defs_h
#define laplace_format_defs_h

#include "../core/unival.h"
#include <functional>
#include <optional>

namespace laplace::format {
  enum class mode { text, binary };

  using pack_type       = std::optional<core::unival>;
  using const_pack_type = core::unival const &;
  using fn_read         = std::function<vbyte(sl::whole)>;
  using fn_write        = std::function<sl::whole(span_cbyte)>;
}

#endif
