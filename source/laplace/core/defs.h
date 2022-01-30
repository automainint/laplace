/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_defs_h
#define laplace_core_defs_h

#include "options.h"

#include "slib.h"

namespace laplace {
  using vbyte   = sl::vector<uint8_t>;
  using vuint16 = sl::vector<uint16_t>;
  using vuint32 = sl::vector<uint32_t>;

  using span_byte   = std::span<uint8_t>;
  using span_index  = std::span<sl::index>;
  using span_uint16 = std::span<uint16_t>;
  using span_uint32 = std::span<uint32_t>;

  using span_cbyte   = std::span<uint8_t const>;
  using span_cindex  = std::span<sl::index const>;
  using span_cuint16 = std::span<uint16_t const>;
  using span_cuint32 = std::span<uint32_t const>;
}

#endif
