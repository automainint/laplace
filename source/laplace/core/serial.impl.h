/*  laplace/core/serial.impl.h
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

#ifndef laplace_core_serial_impl_h
#define laplace_core_serial_impl_h

namespace laplace::serial {
  template <typename type_>
  constexpr auto _str(std::string_view s) noexcept -> type_ {
    auto x = type_ {};

    for (sl::index i = 0; i < s.size(); i++) {
      x <<= 8;
      x |= s[i];
    }

    return x;
  }

  constexpr auto _swap_bytes(auto x) noexcept {
    auto y = static_cast<decltype(x)>(x & 0xff);

    for (sl::index i = 1; i < sizeof x; i++) {
      y <<= 8;
      x >>= 8;
      y |= x & 0xff;
    }

    return y;
  }

  constexpr auto _convert_endian(auto x) noexcept {
    if constexpr (std::endian::native == std::endian::big) {
      return _swap_bytes(x);
    }

    return x;
  }

  template <trivial type_>
  constexpr auto rd(span_cbyte  seq,
                    sl::index   offset,
                    const type_ invalid) noexcept -> type_ {
    auto value = invalid;

    if (offset + sizeof value <= seq.size()) {
      auto dst = &value;
      auto src = seq.data() + offset;

      std::memcpy(dst, src, sizeof value);
    }

    return _convert_endian(value);
  }

  template <trivial type_>
  constexpr void wr(span_byte seq,
                    sl::index offset,
                    type_     value) noexcept {
    if (offset + sizeof value <= seq.size()) {
      auto dst = seq.data() + offset;
      auto src = _convert_endian(value);

      std::memcpy(dst, &src, sizeof value);
    }
  }

  constexpr auto byte_count() noexcept -> sl::whole {
    return 0;
  }

  template <typename arg0_, typename... args_>
  constexpr auto byte_count(arg0_, args_... args) noexcept
      -> sl::whole {
    return sizeof(arg0_) + byte_count(args...);
  }

  template <typename char_type, typename... args_>
  constexpr auto byte_count(std::basic_string_view<char_type> arg0,
                            args_... args) noexcept -> sl::whole {
    return sizeof(char_type) * arg0.size() + byte_count(args...);
  }

  template <typename elem_type, typename... args_>
  constexpr auto byte_count(std::span<const elem_type> arg0,
                            args_... args) noexcept -> sl::whole {
    return sizeof(elem_type) * arg0.size() + byte_count(args...);
  }

  template <trivial char_type_>
  constexpr void write_bytes(
      span_byte                          data,
      std::basic_string_view<char_type_> arg0) noexcept {
    const auto size = sizeof(char_type_) * arg0.size();

    if (data.size() >= size) {
      if constexpr (sizeof(char_type_) == 1) {
        std::memcpy(data.data(), arg0.data(), arg0.size());

      } else {
        for (sl::index i = 0; i < arg0.size(); i++) {
          wr(data, i * sizeof(char_type_), arg0[i]);
        }
      }

    } else {
      error_(fmt("Invalid size %zd. %zd bytes required.", data.size(),
                 size),
             __FUNCTION__);
    }
  }

  template <trivial elem_type_>
  constexpr void write_bytes(
      span_byte data, std::span<const elem_type_> arg0) noexcept {
    const auto size = sizeof(elem_type_) * arg0.size();

    if (data.size() >= size) {
      if constexpr (sizeof(elem_type_) == 1) {
        std::memcpy(data.data(), arg0.data(), arg0.size());

      } else {
        for (sl::index i = 0; i < arg0.size(); i++) {
          wr(data, i * sizeof(elem_type_), arg0.begin()[i]);
        }
      }

    } else {
      error_(fmt("Invalid size %zd. %zd bytes required.", data.size(),
                 size),
             __FUNCTION__);
    }
  }

  template <trivial arg0_trivial_>
  constexpr void write_bytes(span_byte     data,
                             arg0_trivial_ arg0) noexcept {
    constexpr auto size = sizeof arg0;

    if (data.size() >= size) {
      wr(data, 0, arg0);

    } else {
      error_(fmt("Invalid size %zd. %zd bytes required.", data.size(),
                 size),
             __FUNCTION__);
    }
  }

  template <typename arg0_, typename... args_>
  constexpr void write_bytes(span_byte data,
                             arg0_     arg0,
                             args_... args) noexcept {
    const auto size = byte_count(arg0);

    write_bytes(data, arg0);

    if (data.size() >= size) {
      write_bytes(
          span_byte {
              data.begin() +
                  static_cast<decltype(data)::difference_type>(size),
              data.end() },
          args...);

    } else {
      error_(fmt("Invalid size %zd.", data.size()), __FUNCTION__);
    }
  }

  template <trivial... args_trivial_>
  constexpr auto pack_to_array(args_trivial_... args) noexcept {
    constexpr auto size   = (sizeof args + ...);
    auto           result = std::array<uint8_t, size> {};
    write_bytes(result, args...);
    return result;
  }

  template <typename... args_>
  constexpr auto pack_to_bytes(args_... args) noexcept {
    auto result = vbyte(byte_count(args...));
    write_bytes(result, args...);
    return result;
  }
}

#endif
