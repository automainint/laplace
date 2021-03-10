/*  laplace/core/c_utils.cpp
 *
 *      The core utilities implementation.
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

#include "utils.h"
#include <cstdarg>
#include <iostream>
#include <mutex>
#include <sstream>

namespace laplace {
  using std::cout, std::cerr, std::endl, std::unique_lock, std::mutex,
      std::string, std::u8string, std::wstring, std::string_view,
      std::u8string_view, std::wstring_view, std::istringstream,
      std::vector;

  static mutex g_log_mutex;

#ifdef LAPLACE_VERBOSE
  static bool g_verbose = true;

  void verb(string_view s) noexcept {
    if (g_verbose) {
      log(s);
    }
  }

  void verb(const char *c_format, ...) {
    if (g_verbose) {
      va_list ap;
      va_start(ap, c_format);
      verb(to_string(c_format, ap));
      va_end(ap);
    }
  }
#else
  void verb(std::string_view s) noexcept { }
  void verb(const char *c_format, ...) { }
#endif

  void log(string_view s) noexcept {
    auto _lck = unique_lock(g_log_mutex);
    cerr << s << '\n';
  }

  void log(const char *c_format, ...) {
    va_list ap;
    va_start(ap, c_format);
    log(to_string(c_format, ap));
    va_end(ap);
  }

  void error(string_view sender, string_view message) noexcept {
    auto _lck = unique_lock(g_log_mutex);
    cerr << "[ error in " << sender << " ] " << message << '\n';
  }

  void error(string_view sender, const char *c_format, ...) {
    va_list ap;
    va_start(ap, c_format);
    error(sender, to_string(c_format, ap));
    va_end(ap);
  }

  auto to_string(const char *c_format, ...) -> string {
    va_list ap;
    va_start(ap, c_format);
    auto result = to_string(c_format, ap);
    va_end(ap);
    return result;
  }

  auto to_string(const char *c_format, va_list ap) -> string {
    string result;

    if (c_format) {
      const auto size = static_cast<size_t>(
          vsnprintf(nullptr, 0u, c_format, ap));

      result.resize(size);
      vsnprintf(result.data(), size + 1u, c_format, ap);
    }

    return result;
  }

  auto to_u8string(const char8_t *c_format, ...) -> u8string {
    va_list ap;
    va_start(ap, c_format);
    auto result = to_u8string(c_format, ap);
    va_end(ap);
    return result;
  }

  auto to_u8string(const char8_t *c_format, va_list ap) -> u8string {
    return u8string(as_u8string(
        to_string(reinterpret_cast<const char *>(c_format), ap)));
  }

  auto as_ascii_string(u8string_view s) -> string_view {
    return { reinterpret_cast<const char *>(s.data()), s.size() };
  }

  auto as_u8string(string_view s) -> u8string_view {
    return { reinterpret_cast<const char8_t *>(s.data()), s.size() };
  }

  auto to_string(wstring_view s) -> std::string {
    string result;
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
    string   result;
    char32_t c;

    for (size_t i = 0; utf8_decode(s, i, c);) {
      if ((c & 0xffffff00) == 0) {
        result.append(1, static_cast<char>(c));
      } else {
        result.append(1, '_');
      }
    }

    return result;
  }

  auto to_wstring(u8string_view s) -> wstring {
    wstring  result;
    char32_t c;

    for (size_t i = 0; utf8_decode(s, i, c);) {
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

  auto utf8_length(u8string_view bytes) -> size_t {
    size_t   n = 0, i = 0;
    char32_t code = 0;

    for (; utf8_decode(bytes, i, code); n++) { }

    return n;
  }

  auto utf8_decode(u8string_view bytes, ref_uint offset,
                   ref_char32 code) -> bool {
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

  auto utf8_encode(char32_t code, u8string &bytes, ref_uint offset)
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

    vector<uint8_t> temp;
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
