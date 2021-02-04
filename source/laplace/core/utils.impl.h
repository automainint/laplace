/*  laplace/core/utils.impl.h
 *
 *      The inline untility functions implementation.
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

#ifndef __laplace__core_utils_impl__
#define __laplace__core_utils_impl__

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

namespace laplace {
  constexpr auto hex_to_int(char x) -> int {
    int result = 0;

    if (x >= '0' && x <= '9') {
      result = x - '0';
    } else if (x >= 'a' && x <= 'f') {
      result = 10 + (x - 'a');
    } else if (x >= 'A' && x <= 'F') {
      result = 10 + (x - 'A');
    }

    return result;
  }

  constexpr auto int_to_hex(int x) -> char {
    char result = 0;

    if (x < 10) {
      result = '0' + x;
    } else if (x < 16) {
      result = 'a' + (x - 10);
    }

    return result;
  }

  constexpr auto hex_to_int(char16_t x) -> int {
    return hex_to_int(static_cast<char>(x));
  }

  constexpr auto hex_to_int(char32_t x) -> int {
    return hex_to_int(static_cast<char>(x));
  }

  template <typename view_type>
  inline auto to_u8string(view_type s) -> std::u8string {
    std::u8string result;
    size_t        offset = 0;

    for (auto &c : s) { utf8_encode(c, result, offset); }

    return result;
  }

  template <>
  inline auto to_u8string<>(std::string_view s)
      -> std::u8string {
    return std::u8string(s.begin(), s.end());
  }

  template <>
  inline auto to_u8string<>(std::u8string_view s)
      -> std::u8string {
    return std::u8string(s.begin(), s.end());
  }

  template <typename type>
  constexpr auto rd(cref_vbyte seq, size_t offset) -> type {
    type value = 0;

    if (offset + sizeof value <= seq.size()) {
      auto dst = &value;
      auto src = seq.data() + offset;

      std::memcpy(dst, src, sizeof value);
    }

    return value;
  }

  template <typename type>
  constexpr void wr(std::span<uint8_t> seq, size_t offset,
                    type value) {
    if (offset + sizeof value <= seq.size()) {
      auto dst = seq.data() + offset;
      auto src = &value;

      std::memcpy(dst, src, sizeof value);
    }
  }

  template <typename type>
  constexpr void wr(uint8_t *seq, size_t offset, type value) {
    auto dst = seq + offset;
    auto src = &value;

    std::memcpy(dst, src, sizeof value);
  }

  constexpr auto as_index(uint64_t value) -> size_t {
    if constexpr (sizeof(uint64_t) != sizeof(size_t)) {
      if (value > std::numeric_limits<size_t>::max()) {
        verb("Error: 32-bit index value overflow.");
        return -1;
      }
    }

    return static_cast<size_t>(value);
  }

  constexpr auto byte_count() -> size_t {
    return 0;
  }

  template <typename arg0_, typename... args_>
  constexpr auto byte_count(arg0_, args_... args) -> size_t {
    return sizeof(arg0_) + byte_count(args...);
  }

  template <typename char_type, typename... args_>
  constexpr auto
  byte_count(std::basic_string_view<char_type> arg0,
             args_... args) -> size_t {
    return sizeof(char_type) * arg0.size() +
           byte_count(args...);
  }

  template <typename elem_type, typename... args_>
  constexpr auto byte_count(std::span<const elem_type> arg0,
                            args_... args) -> size_t {
    return sizeof(elem_type) * arg0.size() +
           byte_count(args...);
  }

  template <trivial char_type_>
  constexpr void
  write_bytes(std::span<uint8_t>                 data,
              std::basic_string_view<char_type_> arg0) {
    const auto size = sizeof(char_type_) * arg0.size();

    if (data.size() >= size) {
      std::memcpy(data.data(), arg0.data(), size);
    } else {
      error(__FUNCTION__,
            "Invalid size %zd. %zd bytes required.",
            data.size(), size);
    }
  }

  template <trivial elem_type_>
  constexpr void write_bytes(std::span<uint8_t>          data,
                             std::span<const elem_type_> arg0) {
    const auto size = sizeof(elem_type_) * arg0.size();

    if (data.size() >= size) {
      std::memcpy(data.data(), arg0.data(), size);
    } else {
      error(__FUNCTION__,
            "Invalid size %zd. %zd bytes required.",
            data.size(), size);
    }
  }

  template <trivial arg0_trivial_>
  constexpr void write_bytes(std::span<uint8_t> data,
                             arg0_trivial_      arg0) {
    constexpr auto size = sizeof arg0;

    if (data.size() >= size) {
      std::memcpy(data.data(), &arg0, size);
    } else {
      error(__FUNCTION__,
            "Invalid size %zd. %zd bytes required.",
            data.size(), size);
    }
  }

  template <typename arg0_, typename... args_>
  constexpr void write_bytes(std::span<uint8_t> data,
                             arg0_ arg0, args_... args) {
    const auto size = byte_count(arg0);

    write_bytes(data, arg0);

    if (data.size() >= size) {
      write_bytes(std::span<uint8_t> { data.begin() + size,
                                       data.end() },
                  args...);
    } else {
      error(__FUNCTION__, "Invalid size %zd.", data.size());
    }
  }

  template <trivial... args_trivial_>
  constexpr auto pack_to_array(args_trivial_... args) {
    constexpr auto            size = (sizeof args + ...);
    std::array<uint8_t, size> result;
    write_bytes(result, args...);
    return std::move(result);
  }

  template <typename... args_>
  inline auto pack_to_bytes(args_... args) {
    vbyte result(byte_count(args...));
    write_bytes(result, args...);
    return std::move(result);
  }
}

#endif
