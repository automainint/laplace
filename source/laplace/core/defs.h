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

/*  Enable verbose logging.
 */
//#define LAPLACE_VERBOSE

/*  Force to use POSIX sockets.
 */
//#define LAPLACE_POSIX_SOCKETS

/*  Disable platform-dependent features.
 */
//#define LAPLACE_PLATFORM_DUMMY

#ifdef _MSC_VER
#  pragma warning(disable : 4245 4100 4250)
#endif

#include "log.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace laplace {
  /*  TODO
   *  Small vector implementation.
   */
  template <typename type_, typename alloc_ = std::allocator<type_>>
  using small_vector = std::vector<type_, alloc_>;

  using ref_uint   = size_t &;
  using ref_char32 = char32_t &;

  using ref_string  = std::string &;
  using ref_wstring = std::wstring &;

  using vbyte   = small_vector<uint8_t>;
  using vuint   = small_vector<size_t>;
  using vuint16 = small_vector<uint16_t>;
  using vuint32 = small_vector<uint32_t>;

  using span_byte   = std::span<uint8_t>;
  using span_uint   = std::span<size_t>;
  using span_uint16 = std::span<uint16_t>;
  using span_uint32 = std::span<uint32_t>;

  using span_cbyte   = std::span<const uint8_t>;
  using span_cuint   = std::span<const size_t>;
  using span_cuint16 = std::span<const uint16_t>;
  using span_cuint32 = std::span<const uint32_t>;

  using ref_istream = std::istream &;
  using ref_ostream = std::ostream &;

  [[noreturn]] inline void _unreachable() {
#if defined(_MSC_VER)
    __assume(0);
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#endif
  }
}

#endif
