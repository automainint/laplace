/*  Copyright (c) 2022 Mitya Selivanov
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
    auto byte_index = sl::index {};
    auto char_index = sl::index {};
    auto code       = char32_t {};

    for (; decode(bytes, byte_index, code); char_index++) { }

    return char_index;
  }

  auto offset(u8string_view   bytes,
              sl::index const char_count) noexcept -> sl::whole {
    auto byte_index = sl::index {};
    auto char_index = sl::index {};
    auto code       = char32_t {};

    for (; char_index < char_count && decode(bytes, byte_index, code);
         char_index++) { }

    return byte_index;
  }

  auto char_size(u8string_view bytes, sl::index offset) noexcept
      -> sl::whole {
    if (offset < 0 || offset >= bytes.size())
      return 0;
    if ((bytes[offset] & 0x80u) == 0x00u)
      return 1;
    if ((bytes[offset] & 0xc0u) == 0x80u)
      return 0;
    if ((bytes[offset] & 0xe0u) == 0xc0u) {
      if (offset + 1 >= bytes.size())
        return 0;
      if ((bytes[offset + 1] & 0xc0u) != 0x80u)
        return 0;
      return 2;
    }
    if ((bytes[offset] & 0xf0u) == 0xe0u) {
      if (offset + 2 >= bytes.size())
        return 0;
      for (sl::index j = 1; j <= 2; j++)
        if ((bytes[offset + j] & 0xc0u) != 0x80)
          return 0;
      return 3;
    }
    if (offset + 3 >= bytes.size())
      return 0;
    for (sl::index j = 1; j <= 3; j++)
      if ((bytes[offset + j] & 0xc0u) != 0x80u)
        return 0;
    return 4;
  }

  auto next(u8string_view bytes, sl::index offset) noexcept
      -> sl::index {
    return offset + char_size(bytes, offset);
  }

  auto previous(u8string_view bytes, sl::index offset) noexcept
      -> sl::index {
    for (sl::index i = offset - 1; i >= offset - 4; i--)
      if (char_size(bytes, i) != 0)
        return i;
    return offset;
  }

  auto decode(u8string_view bytes, sl::index &offset,
              char32_t &code) noexcept -> bool {
    auto const size = char_size(bytes, offset);

    if (size == 0)
      return false;

    if (size == 1)
      code = bytes[offset];
    if (size == 2)
      code = ((bytes[offset] & 0x1fu) << 6u) |
             (bytes[offset + 1u] & 0x3fu);
    if (size == 3)
      code = ((bytes[offset] & 0x0fu) << 12u) |
             ((bytes[offset + 1u] & 0x3fu) << 6u) |
             (bytes[offset + 2u] & 0x3fu);
    if (size == 4)
      code = ((bytes[offset] & 0x0fu) << 18u) |
             ((bytes[offset + 1u] & 0x3fu) << 12u) |
             ((bytes[offset + 2u] & 0x3fu) << 6u) |
             (bytes[offset + 3u] & 0x3fu);

    offset += size;
    return true;
  }

  auto encode(char32_t code, u8string &bytes,
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

    if (code <= 0x07ffu) {
      temp.emplace_back<uint8_t>(0xc0u | ((code >> 6u) & 0x1fu));
      temp.emplace_back<uint8_t>(0x80u | (code & 0x3fu));
    } else if (code <= 0xffffu) {
      temp.emplace_back<uint8_t>(0xe0u | ((code >> 12u) & 0x0fu));
      temp.emplace_back<uint8_t>(0x80u | ((code >> 6u) & 0x3fu));
      temp.emplace_back<uint8_t>(0x80u | (code & 0x3fu));
    } else if (code <= 0x10ffffu) {
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
