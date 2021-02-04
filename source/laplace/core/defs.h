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

#ifndef __laplace__core_defs__
#define __laplace__core_defs__

#include <array>
#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace laplace {
  using ref_uint   = size_t &;
  using ref_char32 = char32_t &;

  using ref_string  = std::string &;
  using ref_wstring = std::wstring &;

  using vbyte   = std::vector<uint8_t>;
  using vuint   = std::vector<size_t>;
  using vuint16 = std::vector<uint16_t>;
  using vuint32 = std::vector<uint32_t>;

  using ref_vbyte   = std::span<uint8_t>;
  using ref_vuint   = std::span<size_t>;
  using ref_vuint16 = std::span<uint16_t>;
  using ref_vuint32 = std::span<uint32_t>;

  using cref_vbyte   = std::span<const uint8_t>;
  using cref_vuint   = std::span<const size_t>;
  using cref_vuint16 = std::span<const uint16_t>;
  using cref_vuint32 = std::span<const uint32_t>;
  using cref_vstring = std::span<const std::string>;

  using ref_istream = std::istream &;
  using ref_ostream = std::ostream &;

  void log(std::string_view s);
  void log(const char *c_format, ...);

  void error(std::string_view sender, std::string_view message);
  void error(
      std::string_view sender, const char *c_format, ...);

  void verb(std::string_view s);
  void verb(const char *c_format, ...);
}

#endif
