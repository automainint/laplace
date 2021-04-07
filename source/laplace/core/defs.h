/*  laplace/core/defs.h
 *
 *      The core definitions. Type definitions and log
 *      functions.
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

#ifndef laplace_core_defs_h
#define laplace_core_defs_h

#include "options.h"

#include "slib.h"

namespace laplace {
  using ref_uint   = size_t &;
  using ref_char32 = char32_t &;

  using ref_string  = std::string &;
  using ref_wstring = std::wstring &;

  using vbyte   = sl::vector<uint8_t>;
  using vindex  = sl::vector<sl::index>;
  using vuint   = sl::vector<size_t>;
  using vuint16 = sl::vector<uint16_t>;
  using vuint32 = sl::vector<uint32_t>;

  using span_byte   = std::span<uint8_t>;
  using span_index  = std::span<sl::index>;
  using span_uint   = std::span<size_t>;
  using span_uint16 = std::span<uint16_t>;
  using span_uint32 = std::span<uint32_t>;

  using span_cbyte   = std::span<const uint8_t>;
  using span_cindex  = std::span<const sl::index>;
  using span_cuint   = std::span<const size_t>;
  using span_cuint16 = std::span<const uint16_t>;
  using span_cuint32 = std::span<const uint32_t>;
}

#endif
