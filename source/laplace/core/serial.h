/*  laplace/core/serial.h
 *
 *      The serialization features.
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

#ifndef laplace_core_serial_h
#define laplace_core_serial_h

#include "defs.h"
#include "string.h"
#include <concepts>

namespace laplace::serial {
  template <typename type>
  concept trivial = std::is_trivial_v<type>;

  /*  Read value of specified type from specified offset.
   */
  template <trivial type_>
  [[nodiscard]] constexpr auto rd(span_cbyte seq, size_t offset) noexcept
      -> type_;

  /*  Write the value to specified offset.
   */
  template <trivial type_>
  constexpr void wr(std::span<uint8_t> seq, size_t offset,
                    type_ value) noexcept;

  template <trivial type_>
  constexpr void wr(uint8_t *seq, size_t offset, type_ value) noexcept;

  [[nodiscard]] constexpr auto byte_count() noexcept -> size_t;

  template <typename arg0_, typename... args_>
  [[nodiscard]] constexpr auto byte_count(arg0_ arg0,
                                          args_... args) noexcept
      -> size_t;

  template <typename char_type, typename... args_>
  [[nodiscard]] constexpr auto byte_count(
      std::basic_string_view<char_type> arg0, args_... args) noexcept
      -> size_t;

  template <typename elem_type, typename... args_>
  [[nodiscard]] constexpr auto byte_count(std::span<const elem_type> arg0,
                                          args_... args) noexcept
      -> size_t;

  template <trivial char_type_>
  constexpr void write_bytes(
      std::span<uint8_t>                 data,
      std::basic_string_view<char_type_> arg0) noexcept;

  template <trivial elem_type_>
  constexpr void write_bytes(std::span<uint8_t>          data,
                             std::span<const elem_type_> arg0) noexcept;

  template <trivial arg0_trivial_>
  constexpr void write_bytes(std::span<uint8_t> data,
                             arg0_trivial_      arg0) noexcept;

  template <typename arg0_, typename... args_>
  constexpr void write_bytes(std::span<uint8_t> data, arg0_ arg0,
                             args_... args) noexcept;

  template <trivial... args_trivial_>
  [[nodiscard]] constexpr auto pack_to_array(args_trivial_... args) noexcept;

  template <typename... args_>
  [[nodiscard]] constexpr auto pack_to_bytes(args_... args) noexcept;
}

#include "serial.impl.h"

#endif
