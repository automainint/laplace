/*  laplace/core/c_string.cpp
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

#include "string.h"
#include "utf8.h"
#include <cstdarg>
#include <sstream>

namespace laplace {
  using std::string, std::u8string, std::wstring, std::string_view,
      std::u8string_view, std::wstring_view, std::istringstream,
      utf8::decode, utf8::encode;

  auto fmt(const char *c_format, ...) -> string {
    va_list ap;
    va_start(ap, c_format);
    auto result = fmt(c_format, ap);
    va_end(ap);
    return result;
  }

  auto fmt(const char *c_format, va_list ap) -> string {
    if (c_format) {
      const auto size = static_cast<size_t>(
          vsnprintf(nullptr, 0u, c_format, ap));

      auto result = std::vector<char>(size + 2, '\0');
      vsnprintf(result.data(), size + 1, c_format, ap);
      result.back() = '\0';
      return result.data();
    }

    return {};
  }

  auto fmt(const char8_t *c_format, ...) -> u8string {
    va_list ap;
    va_start(ap, c_format);
    auto result = fmt(c_format, ap);
    va_end(ap);
    return result;
  }

  auto fmt(const char8_t *c_format, va_list ap) -> u8string {
    return u8string(as_u8string(
        fmt(reinterpret_cast<const char *>(c_format), ap)));
  }

  auto as_ascii_string(u8string_view s) noexcept -> string_view {
    return { reinterpret_cast<const char *>(s.data()), s.size() };
  }

  auto as_u8string(string_view s) noexcept -> u8string_view {
    return { reinterpret_cast<const char8_t *>(s.data()), s.size() };
  }

  auto to_string(wstring_view s) -> std::string {
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

  auto to_string(u8string_view s) -> string {
    auto result = string {};
    auto c      = char32_t {};

    for (size_t i = 0; decode(s, i, c);) {
      if ((c & 0xffffff00) == 0) {
        result.append(1, static_cast<char>(c));
      } else {
        result.append(1, '_');
      }
    }

    return result;
  }

  auto to_wstring(u8string_view s) -> wstring {
    auto result = wstring {};
    auto c      = char32_t {};

    for (size_t i = 0; decode(s, i, c);) {
      if ((c & 0xffff0000) == 0) {
        result.append(1, static_cast<wchar_t>(c));
      } else {
        result.append(1, L'_');
      }
    }

    return result;
  }

  auto to_integer(std::string_view s) -> int64_t {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    int64_t n;
    ss >> n;

    return n;
  }

  auto to_integer(u8string_view s) -> int64_t {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    int64_t n;
    ss >> n;

    return n;
  }

  auto to_uint(std::string_view s) -> uint64_t {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    uint64_t n;
    ss >> n;

    return n;
  }

  auto to_uint(u8string_view s) -> uint64_t {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    uint64_t n;
    ss >> n;

    return n;
  }

  auto to_real(string_view s) -> double {
    auto buf = string(s);
    auto ss  = istringstream(buf);

    double f;
    ss >> f;

    return f;
  }

  auto to_real(std::u8string_view s) -> double {
    auto buf = string(as_ascii_string(s));
    auto ss  = istringstream(buf);

    double f;
    ss >> f;

    return f;
  }
}
