/*  laplace/core/c_utf8.cpp
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

#include "utf8.h"

namespace laplace::utf8 {
  using std::u8string_view, std::u8string, std::vector;

  auto length(u8string_view bytes) noexcept -> size_t {
    size_t   n = 0, i = 0;
    char32_t code = 0;

    for (; decode(bytes, i, code); n++) { }

    return n;
  }

  auto decode(u8string_view bytes, ref_uint offset,
              ref_char32 code) noexcept -> bool {
    if (offset >= bytes.size() || bytes[offset] == 0xFF) {
      return false;
    } else if ((bytes[offset] & 0x80) == 0x00) {
      code = bytes[offset];
      offset++;
    } else if ((bytes[offset] & 0xC0) == 0x80) {
      return false;
    } else if ((bytes[offset] & 0xE0) == 0xC0) {
      if (offset + 1 >= bytes.size()) {
        return false;
      }

      if ((bytes[offset + 1] & 0xC0) != 0x80) {
        return false;
      }

      code = ((bytes[offset] & 0x1Fu) << 6u) |
             (bytes[offset + 1u] & 0x3Fu);

      offset += 2;
    } else if ((bytes[offset] & 0xF0) == 0xE0) {
      if (offset + 2 >= bytes.size()) {
        return false;
      }

      for (size_t j = 1; j < 2; j++) {
        if ((bytes[offset + j] & 0xC0) != 0x80) {
          return false;
        }
      }

      code = ((bytes[offset] & 0x0Fu) << 12u) |
             ((bytes[offset + 1u] & 0x3Fu) << 6u) |
             (bytes[offset + 2u] & 0x3Fu);

      offset += 3;
    } else if ((bytes[offset] & 0xF8) == 0xF0) {
      if (offset + 3 >= bytes.size()) {
        return false;
      }

      for (size_t j = 1; j < 3; j++) {
        if ((bytes[offset + j] & 0xC0) != 0x80) {
          return false;
        }
      }

      code = ((bytes[offset] & 0x0Fu) << 18u) |
             ((bytes[offset + 1u] & 0x3Fu) << 12u) |
             ((bytes[offset + 2u] & 0x3Fu) << 6u) |
             (bytes[offset + 3u] & 0x3Fu);

      offset += 4;
    }

    return true;
  }

  auto encode(char32_t code, u8string &bytes, ref_uint offset) noexcept
      -> bool {
    if (offset > bytes.size()) {
      return false;
    }

    if (code <= 0x7Fu) {
      bytes.insert(bytes.begin() + static_cast<ptrdiff_t>(offset),
                   static_cast<uint8_t>(code));
      offset++;
      return true;
    }

    auto temp = vector<uint8_t> {};
    temp.reserve(4);

    if (code <= 0x07FF) {
      temp.emplace_back(
          static_cast<uint8_t>(0xC0 | ((code >> 6) & 0x1F)));
      temp.emplace_back(static_cast<uint8_t>(0x80 | (code & 0x3F)));
    } else if (code <= 0xFFFF) {
      temp.emplace_back(
          static_cast<uint8_t>(0xE0 | ((code >> 12) & 0x0F)));
      temp.emplace_back(
          static_cast<uint8_t>(0x80 | ((code >> 6) & 0x3F)));
      temp.emplace_back(static_cast<uint8_t>(0x80 | (code & 0x3F)));
    } else if (code <= 0x10FFFF) {
      temp.emplace_back(
          static_cast<uint8_t>(0xF0 | ((code >> 18) & 0x0E)));
      temp.emplace_back(
          static_cast<uint8_t>(0x80 | ((code >> 12) & 0x3F)));
      temp.emplace_back(
          static_cast<uint8_t>(0x80 | ((code >> 6) & 0x3F)));
      temp.emplace_back(static_cast<uint8_t>(0x80 | (code & 0x3F)));
    } else {
      return false;
    }

    bytes.insert(bytes.begin() + static_cast<ptrdiff_t>(offset),
                 temp.begin(), temp.end());
    offset += temp.size();

    return true;
  }
}
