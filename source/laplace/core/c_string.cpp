/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "string.h"

#include "utf8.h"
#include <cstdarg>
#include <sstream>

namespace laplace {
  using std::string, std::u8string, std::wstring, std::string_view,
      std::u8string_view, std::wstring_view, std::istringstream,
      utf8::decode;

  auto fmt(const char *c_format, ...) noexcept -> string {
    va_list ap;
    va_start(ap, c_format);
    auto result = fmt(c_format, ap);
    va_end(ap);
    return result;
  }

  auto fmt(const char *c_format, va_list ap) noexcept -> string {
    if (c_format && ap) {
      auto buf = std::array<char, 0x1000> {};

      if (vsnprintf(buf.data(), buf.size() - 1, c_format, ap) < 0)
        return {};

      return string(buf.data());
    }

    return {};
  }

  auto fmt(const char8_t *c_format, ...) noexcept -> u8string {
    va_list ap;
    va_start(ap, c_format);
    auto result = fmt(c_format, ap);
    va_end(ap);
    return result;
  }

  auto fmt(const char8_t *c_format, va_list ap) noexcept -> u8string {
    return u8string(as_u8string(
        fmt(reinterpret_cast<const char *>(c_format), ap)));
  }

  auto as_ascii_string(u8string_view s) noexcept -> string_view {
    return { reinterpret_cast<const char *>(s.data()), s.size() };
  }

  auto as_u8string(string_view s) noexcept -> u8string_view {
    return { reinterpret_cast<const char8_t *>(s.data()), s.size() };
  }

  auto to_string(wstring_view s) noexcept -> std::string {
    auto result = string {};
    result.reserve(s.size());

    for (const auto c : s) {
      if ((c & 0xff00) == 0) {
        result.append(1, static_cast<char>(c));
      } else {
        result.append(1, '_');
      }
    }

    return result;
  }

  auto to_string(u8string_view s) noexcept -> string {
    auto result = string {};
    auto c      = char32_t {};

    for (sl::index i = 0; decode(s, i, c);) {
      if ((c & 0xffffff00) == 0) {
        result.append(1, static_cast<char>(c));
      } else {
        result.append(1, '_');
      }
    }

    return result;
  }

  auto to_wstring(u8string_view s) noexcept -> wstring {
    auto result = wstring {};
    auto c      = char32_t {};

    for (sl::index i = 0; decode(s, i, c);) {
      if ((c & 0xffff0000) == 0) {
        result.append(1, static_cast<wchar_t>(c));
      } else {
        result.append(1, L'_');
      }
    }

    return result;
  }

  auto to_integer(std::string_view s) noexcept -> int64_t {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    auto n = int64_t {};
    ss >> n;

    return n;
  }

  auto to_integer(u8string_view s) noexcept -> int64_t {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    auto n = int64_t {};
    ss >> n;

    return n;
  }

  auto to_uint(std::string_view s) noexcept -> uint64_t {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    auto n = uint64_t {};
    ss >> n;

    return n;
  }

  auto to_uint(u8string_view s) noexcept -> uint64_t {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    auto n = uint64_t {};
    ss >> n;

    return n;
  }

  auto to_real(string_view s) noexcept -> double {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    auto f = double {};
    ss >> f;

    return f;
  }

  auto to_real(std::u8string_view s) noexcept -> double {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    auto f = double {};
    ss >> f;

    return f;
  }
}
