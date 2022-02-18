/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_string_h
#define laplace_core_string_h

#include "defs.h"
#include "utf8.h"

namespace laplace {
  [[nodiscard]] constexpr auto hex_to_int(char x) noexcept -> int;
  [[nodiscard]] constexpr auto int_to_hex(int x) noexcept -> char;

  [[nodiscard]] constexpr auto hex_to_int(char16_t x) noexcept -> int;
  [[nodiscard]] constexpr auto hex_to_int(char32_t x) noexcept -> int;

  /*  TODO
   *  Use C++20 format library.
   */
  [[nodiscard]] auto fmt(const char *c_format, ...) noexcept
      -> std::string;

  [[nodiscard]] auto fmt(const char *c_format, va_list ap) noexcept
      -> std::string;

  [[nodiscard]] auto fmt(const char8_t *c_format, ...) noexcept
      -> std::u8string;

  [[nodiscard]] auto fmt(const char8_t *c_format, va_list ap) noexcept
      -> std::u8string;

  [[nodiscard]] auto as_ascii_string(std::u8string_view s) noexcept
      -> std::string_view;

  [[nodiscard]] auto as_u8string(std::string_view s) noexcept
      -> std::u8string_view;

  [[nodiscard]] auto to_string(std::wstring_view s) noexcept
      -> std::string;

  [[nodiscard]] auto to_string(std::u8string_view s) noexcept
      -> std::string;

  [[nodiscard]] auto to_wstring(std::u8string_view s) noexcept
      -> std::wstring;

  [[nodiscard]] auto to_integer(std::string_view s) noexcept
      -> int64_t;

  [[nodiscard]] auto to_integer(std::u8string_view s) noexcept
      -> int64_t;

  [[nodiscard]] auto to_uint(std::string_view s) noexcept -> uint64_t;

  [[nodiscard]] auto to_uint(std::u8string_view s) noexcept
      -> uint64_t;

  [[nodiscard]] auto to_real(std::string_view s) noexcept -> double;

  [[nodiscard]] auto to_real(std::u8string_view s) noexcept -> double;

  template <typename view_type>
  [[nodiscard]] auto to_u8string(view_type s, log_handler log = get_global_log()) noexcept
      -> std::u8string;
}

#include "string.impl.h"

#endif
