/*  laplace/core/utils.h
 *
 *      The core utilities.
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

#ifndef laplace_core_utils_h
#define laplace_core_utils_h

#include "bytestream.h"
#include "defs.h"
#include <concepts>

namespace laplace {
  template <typename type>
  concept trivial = std::is_trivial_v<type>;

  [[nodiscard]] auto to_string(const char *c_format, ...)
      -> std::string;

  [[nodiscard]] auto to_string(const char *c_format, va_list ap)
      -> std::string;

  [[nodiscard]] auto to_u8string(const char8_t *c_format, ...)
      -> std::u8string;

  [[nodiscard]] auto to_u8string(const char8_t *c_format, va_list ap)
      -> std::u8string;

  [[nodiscard]] constexpr auto hex_to_int(char x) -> int;
  [[nodiscard]] constexpr auto int_to_hex(int x) -> char;

  [[nodiscard]] constexpr auto hex_to_int(char16_t x) -> int;
  [[nodiscard]] constexpr auto hex_to_int(char32_t x) -> int;

  [[nodiscard]] auto as_ascii_string(std::u8string_view s)
      -> std::string;

  [[nodiscard]] auto as_u8string(std::string_view s) -> std::u8string;

  [[nodiscard]] auto to_string(std::wstring_view s) -> std::string;

  [[nodiscard]] auto to_string(std::u8string_view s) -> std::string;

  [[nodiscard]] auto to_wstring(std::u8string_view s) -> std::wstring;

  [[nodiscard]] auto to_integer(std::string_view s) -> int64_t;

  [[nodiscard]] auto to_integer(std::u8string_view s) -> int64_t;

  [[nodiscard]] auto to_real(std::string_view s) -> double;

  [[nodiscard]] auto to_real(std::u8string_view s) -> double;

  [[nodiscard]] auto utf8_length(std::u8string_view bytes) -> size_t;

  [[nodiscard]] auto utf8_decode(std::u8string_view bytes,
                                 ref_uint offset, ref_char32 code)
      -> bool;

  [[nodiscard]] auto utf8_encode(char32_t code, std::u8string &bytes,
                                 ref_uint offset) -> bool;

  template <typename view_type>
  [[nodiscard]] auto to_u8string(view_type s) -> std::u8string;

  /*  Read value of specified type from specified offset.
   */
  template <typename type>
  [[nodiscard]] constexpr auto rd(cref_vbyte seq, size_t offset)
      -> type;

  /*  Write the value to specified offset.
   */
  template <typename type>
  constexpr void wr(std::span<uint8_t> seq, size_t offset, type value);

  template <typename type>
  constexpr void wr(uint8_t *seq, size_t offset, type value);

  /*  Convert unsigned integer value to an index.
   *  All indices are size_t values.
   *
   *  Returns -1 for invalid index.
   */
  [[nodiscard]] constexpr auto as_index(uint64_t value) -> size_t;

  [[nodiscard]] constexpr auto byte_count() -> size_t;

  template <typename arg0_, typename... args_>
  [[nodiscard]] constexpr auto byte_count(arg0_ arg0, args_... args)
      -> size_t;

  template <typename char_type, typename... args_>
  [[nodiscard]] constexpr auto byte_count(
      std::basic_string_view<char_type> arg0, args_... args) -> size_t;

  template <typename elem_type, typename... args_>
  [[nodiscard]] constexpr auto byte_count(std::span<const elem_type> arg0,
                                          args_... args) -> size_t;

  template <trivial char_type_>
  constexpr void write_bytes(std::span<uint8_t>                 data,
                             std::basic_string_view<char_type_> arg0);

  template <trivial elem_type_>
  constexpr void write_bytes(std::span<uint8_t>          data,
                             std::span<const elem_type_> arg0);

  template <trivial arg0_trivial_>
  constexpr void write_bytes(std::span<uint8_t> data,
                             arg0_trivial_      arg0);

  template <typename arg0_, typename... args_>
  constexpr void write_bytes(std::span<uint8_t> data, arg0_ arg0,
                             args_... args);

  template <trivial... args_trivial_>
  [[nodiscard]] constexpr auto pack_to_array(args_trivial_... args);

  template <typename... args_>
  [[nodiscard]] auto pack_to_bytes(args_... args);
}

#include "utils.impl.h"

#endif
