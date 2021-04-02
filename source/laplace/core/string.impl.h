/*  laplace/core/string.impl.h
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

#ifndef laplace_core_string_impl_h
#define laplace_core_string_impl_h

namespace laplace {
  constexpr auto hex_to_int(char x) noexcept -> int {
    auto result = int {};

    if (x >= '0' && x <= '9') {
      result = x - '0';
    } else if (x >= 'a' && x <= 'f') {
      result = 10 + (x - 'a');
    } else if (x >= 'A' && x <= 'F') {
      result = 10 + (x - 'A');
    }

    return result;
  }

  constexpr auto int_to_hex(int x) noexcept -> char {
    if (x >= 0 && x < 10) {
      return static_cast<char>('0' + x);
    }

    if (x >= 10 && x < 16) {
      return static_cast<char>('a' + (x - 10));
    }

    return '\0';
  }

  constexpr auto hex_to_int(char16_t x) noexcept -> int {
    return hex_to_int(static_cast<char>(x));
  }

  constexpr auto hex_to_int(char32_t x) noexcept -> int {
    return hex_to_int(static_cast<char>(x));
  }

  template <typename view_type>
  inline auto to_u8string(view_type s) -> std::u8string {
    auto result = std::u8string {};
    auto offset = size_t {};

    for (auto &c : s) {
      if (!utf8::encode(c, result, offset)) {
        error_("Unable to encode UTF-8 string.", __FUNCTION__);
        break;
      }
    }

    return result;
  }

  template <>
  inline auto to_u8string<>(std::string_view s) -> std::u8string {
    return std::u8string(                            //
        reinterpret_cast<const char8_t *>(s.data()), //
        s.size());
  }

  template <>
  inline auto to_u8string<>(std::u8string_view s) -> std::u8string {
    return std::u8string(s.begin(), s.end());
  }
}

#endif
