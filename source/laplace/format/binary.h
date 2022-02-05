/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_format_binary_h
#define laplace_format_binary_h

#include "defs.h"

namespace laplace::format::binary {
  namespace ids {
    enum _id : uint8_t {
      empty,
      bool_true,
      bool_false,
      integer,
      string,
      uint,
      real,
      bytes,
      bitfield,
      vector,
      composite,
      compact_composite,
      packed_string,
      packed_composite
    };
  }

  [[nodiscard]] auto decode(fn_read read) noexcept -> pack_type;
  [[nodiscard]] auto encode(fn_write        write,
                            const_pack_type data) noexcept -> bool;
}

#endif
