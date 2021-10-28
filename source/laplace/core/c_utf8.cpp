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
  using std::u8string_view, std::u8string;

  auto length(u8string_view bytes) noexcept -> sl::whole {
    auto n    = sl::index {};
    auto i    = sl::index {};
    auto code = char32_t {};

    for (; decode(bytes, i, code); n++) { }

    return n;
  }

  auto decode(u8string_view bytes,
              sl::index    &offset,
              char32_t     &code) noexcept -> bool {
    if (offset < 0 || offset >= bytes.size() ||
        (bytes[offset] & 0xf8) == 0xf8)
      return false;

    if ((bytes[offset] & 0x80) == 0x00) {
      code = bytes[offset];
      offset++;
    } else if ((bytes[offset] & 0xc0) == 0x80) {
      return false;
    } else if ((bytes[offset] & 0xe0) == 0xc0) {
      if (offset + 1 >= bytes.size())
        return false;

      if ((bytes[offset + 1] & 0xc0) != 0x80)
        return false;

      code = ((bytes[offset] & 0x1fu) << 6u) |
             (bytes[offset + 1u] & 0x3fu);

      offset += 2;
    } else if ((bytes[offset] & 0xf0) == 0xe0) {
      if (offset + 2 >= bytes.size())
        return false;

      for (sl::index j = 1; j <= 2; j++)
        if ((bytes[offset + j] & 0xc0) != 0x80)
          return false;

      code = ((bytes[offset] & 0x0fu) << 12u) |
             ((bytes[offset + 1u] & 0x3fu) << 6u) |
             (bytes[offset + 2u] & 0x3fu);

      offset += 3;
    } else {
      if (offset + 3 >= bytes.size())
        return false;

      for (sl::index j = 1; j <= 3; j++)
        if ((bytes[offset + j] & 0xc0) != 0x80)
          return false;

      code = ((bytes[offset] & 0x0fu) << 18u) |
             ((bytes[offset + 1u] & 0x3fu) << 12u) |
             ((bytes[offset + 2u] & 0x3fu) << 6u) |
             (bytes[offset + 3u] & 0x3fu);

      offset += 4;
    }

    return true;
  }

  auto encode(char32_t   code,
              u8string  &bytes,
              sl::index &offset) noexcept -> bool {
    if (offset < 0 || offset > bytes.size())
      return false;

    if (code <= 0x7fu) {
      bytes.insert(bytes.begin() + offset,
                   static_cast<uint8_t>(code));
      offset++;
      return true;
    }

    auto temp = sl::vector<uint8_t> {};
    temp.reserve(4);

    if (code <= 0x07ff) {
      temp.emplace_back<uint8_t>(0xc0u | ((code >> 6u) & 0x1fu));
      temp.emplace_back<uint8_t>(0x80u | (code & 0x3fu));
    } else if (code <= 0xffff) {
      temp.emplace_back<uint8_t>(0xe0u | ((code >> 12u) & 0x0fu));
      temp.emplace_back<uint8_t>(0x80u | ((code >> 6u) & 0x3fu));
      temp.emplace_back<uint8_t>(0x80u | (code & 0x3fu));
    } else if (code <= 0x10ffff) {
      temp.emplace_back<uint8_t>(0xf0u | ((code >> 18u) & 0x0eu));
      temp.emplace_back<uint8_t>(0x80u | ((code >> 12u) & 0x3fu));
      temp.emplace_back<uint8_t>(0x80u | ((code >> 6u) & 0x3fu));
      temp.emplace_back<uint8_t>(0x80u | (code & 0x3fu));
    } else {
      return false;
    }

    bytes.insert(bytes.begin() + offset, temp.begin(), temp.end());
    offset += temp.size();

    return true;
  }
}
