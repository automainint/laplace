/*  laplace/format/binary.h
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

#ifndef laplace_format_binary_h
#define laplace_format_binary_h

#include "defs.h"

namespace laplace::format::binary {
  namespace ids {
    enum idval : uint8_t {
      empty,
      bool_true,
      bool_false,
      integer,
      string,
      uint,
      bytes,
      bitfield,
      vector,
      composite,
      compact_composite
    };
  }

  /*  Replace string key ids with
   *  numeric synonyms.
   */
  void pack(core::ref_family data);

  /*  Replace numeric key ids back
   *  to strings.
   */
  void unpack(core::ref_family data);

  auto decode(fn_read read) -> pack_type;
  auto encode(fn_write write, const_pack_type data) -> bool;

  /*  String key ids table.
   */
  extern const std::vector<std::u8string> table;
}

#endif
