/*  laplace/format/f_binary.cpp
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

/*  TODO
 *  Add bzip2 compression support.
 */

#include "binary.h"

#include "../core/serial.h"
#include "../core/utils.h"
#include "text.h"
#include <algorithm>
#include <cmath>

namespace laplace::format::binary {
  using std::make_shared, std::lower_bound, std::u8string,
      std::u8string_view, core::ref_family, core::cref_family,
      core::family, serial::rd, serial::wr;

  static constexpr auto _sorted(auto v) {
    std::sort(v.begin(), v.end());
    return v;
  }

  static constexpr auto dictionary = _sorted(
      std::array<u8string_view, 8> {
          text::s_function, text::s_arguments, text::s_commands,
          u8"load", u8"width", u8"height", u8"depth", u8"pixels" });

  static constexpr auto magic = serial::_str<uint64_t>("laplace0");

  static constexpr auto mantissa_factor = 1e17;

  static auto is_packable(cref_family value) noexcept -> bool {
    if (!value.is_composite()) {
      return false;
    }

    for (sl::index i = 0; i < value.get_size(); i++) {
      if (!value.get_key(i).is_string()) {
        return false;
      }

      if (auto j = lower_bound(dictionary.begin(), dictionary.end(),
                               value.get_string());
          j == dictionary.end() || *j != value.get_string()) {
        return false;
      }
    }

    return true;
  }

  static auto read_int(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8)
      return false;

    value = rd<int64_t>(v, 0);
    return true;
  }

  static auto read_string(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8)
      return false;

    auto size = as_index(rd<int64_t>(v, 0));
    v         = read(size);

    if (v.size() != size)
      return false;

    value = u8string { v.begin(), v.end() };
    return true;
  }

  static auto read_uint(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8)
      return false;

    value = rd<uint64_t>(v, 0);
    return true;
  }

  static auto read_real(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(12);

    if (v.size() != 12)
      return false;

    auto m        = rd<int64_t>(v, 0);
    auto exponent = rd<int32_t>(v, 8);
    auto mantissa = static_cast<long double>(m) / mantissa_factor;

    value = static_cast<double>(ldexp(mantissa, exponent));

    return true;
  }

  static auto read_bytes(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8)
      return false;

    auto size = as_index(rd<int64_t>(v, 0));
    v         = read(size);

    if (v.size() != size)
      return false;

    value = v;
    return true;
  }

  static auto read_bitfield(fn_read read, ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8)
      return false;

    const auto size       = rd<int64_t>(v, 0);
    const auto byte_count = (size + 7) / 8;

    v = read(byte_count);

    if (v.size() != byte_count)
      return false;

    value = family {};

    auto i = sl::index {};
    auto n = sl::index {};
    auto k = sl::index {};

    for (i = 0, n = 0; i < size; n++) {
      for (k = 0; k < 8 && i < size; k++, i++) {
        if (n >= v.size()) {
          return false;
        }

        value[i] = (v[n] & (0x80 >> k)) ? true : false;
      }
    }

    return true;
  }

  static auto read_pack(fn_read read, ref_family value) noexcept
      -> bool;

  static auto read_vector(fn_read read, ref_family value) noexcept
      -> bool {
    const auto v = read(8);

    if (v.size() != 8) {
      return false;
    }

    const auto size = as_index(rd<int64_t>(v, 0));

    value = family {};

    for (sl::index i = 0; i < size; i++) {
      auto field = family {};

      if (!read_pack(read, field)) {
        return false;
      }

      value[i] = field;
    }

    return true;
  }

  static auto read_composite(fn_read read, ref_family value) noexcept
      -> bool {
    const auto v = read(8);

    if (v.size() != 8) {
      return false;
    }

    auto size = as_index(rd<int64_t>(v, 0));

    value = family {};

    for (sl::index i = 0; i < size; i++) {
      auto key   = family {};
      auto field = family {};

      if (!read_pack(read, key))
        return false;
      if (!read_pack(read, field))
        return false;

      value[key] = field;
    }

    return true;
  }

  static auto read_compact_composite(fn_read    read,
                                     ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8) {
      return false;
    }

    auto size = as_index(rd<int64_t>(v, 0));

    value = family {};

    for (sl::index i = 0; i < size; i++) {
      auto field = family {};

      v = read(8);

      if (v.size() != 8) {
        return false;
      }

      auto n = as_index(rd<int64_t>(v, 0));

      if (n < 0) {
        return false;
      }

      if (!read_pack(read, field)) {
        return false;
      }

      value.by_key(n) = field;
    }

    return true;
  }

  static auto read_packed_string(fn_read    read,
                                 ref_family value) noexcept -> bool {
    auto v = read(8);

    if (v.size() != 8) {
      return false;
    }

    auto n = as_index(rd<int64_t>(v, 0));

    if (n < 0 || n >= dictionary.size()) {
      return false;
    }

    value = dictionary[n];
    return true;
  }

  static auto read_packed_composite(fn_read    read,
                                    ref_family value) noexcept
      -> bool {
    auto v = read(8);

    if (v.size() != 8) {
      return false;
    }

    auto size = as_index(rd<int64_t>(v, 0));

    value = family {};

    for (sl::index i = 0; i < size; i++) {
      auto field = family {};

      v = read(8);

      if (v.size() != 8) {
        return false;
      }

      auto n = as_index(rd<int64_t>(v, 0));

      if (n < 0 || n >= dictionary.size()) {
        return false;
      }

      if (!read_pack(read, field)) {
        return false;
      }

      value[u8string_view { dictionary[n] }] = field;
    }

    return true;
  }

  static auto read_pack(fn_read read, ref_family value) noexcept
      -> bool {
    const auto v = read(1);

    if (v.size() != 1) {
      return false;
    }

    const auto id = v[0];

    if (id == ids::empty) {
      value = family {};
      return true;
    }

    if (id == ids::bool_true) {
      value = true;
      return true;
    }

    if (id == ids::bool_false) {
      value = false;
      return true;
    }

    if (id == ids::integer)
      return read_int(read, value);
    if (id == ids::string)
      return read_string(read, value);
    if (id == ids::uint)
      return read_uint(read, value);
    if (id == ids::real)
      return read_real(read, value);
    if (id == ids::bytes)
      return read_bytes(read, value);
    if (id == ids::bitfield)
      return read_bitfield(read, value);

    if (id == ids::vector)
      return read_vector(read, value);
    if (id == ids::composite)
      return read_composite(read, value);

    if (id == ids::compact_composite)
      return read_compact_composite(read, value);
    if (id == ids::packed_string)
      return read_packed_string(read, value);
    if (id == ids::packed_composite)
      return read_packed_composite(read, value);

    return false;
  }

  auto decode(fn_read read) noexcept -> pack_type {
    if (!read)
      return {};

    auto const v = read(8);

    if (v.size() != 8)
      return {};

    if (rd<uint64_t>(v, 0) != magic)
      return {};

    if (auto f = family {}; read_pack(read, f))
      return f;

    return {};
  }

  static auto write_empty(fn_write write) noexcept -> bool {
    return write(vbyte { ids::empty }) == 1;
  }

  static auto write_bool(fn_write write, bool value) noexcept
      -> bool {
    return write(vbyte { value ? ids::bool_true
                               : ids::bool_false }) == 1;
  }

  static auto write_int(fn_write write, int64_t value) noexcept
      -> bool {
    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::integer);
    wr<int64_t>(v, 1, value);
    return write(v) == v.size();
  }

  static auto write_packed_string(fn_write  write,
                                  sl::index n) noexcept -> bool {
    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::packed_string);
    wr<int64_t>(v, 1, n);
    return write(v) == v.size();
  }

  static auto write_string(fn_write      write,
                           u8string_view value) noexcept -> bool {

    if (auto i = lower_bound(dictionary.begin(), dictionary.end(),
                             value);
        i != dictionary.end() && *i == value) {
      return write_packed_string(write, i - dictionary.begin());
    }

    auto v = vbyte(9 + value.size());
    wr<uint8_t>(v, 0, ids::string);
    wr<uint64_t>(v, 1, value.size());
    serial::write_bytes({ v.begin() + 9, v.end() }, value);
    return write(v) == v.size();
  }

  static auto write_uint(fn_write write, uint64_t value) noexcept
      -> bool {
    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::uint);
    wr<uint64_t>(v, 1, value);
    return write(v) == v.size();
  }

  static auto write_real(fn_write write, double value) noexcept
      -> bool {
    auto exponent = int {};
    auto mantissa = frexp(value, &exponent) * mantissa_factor;
    auto m        = static_cast<int64_t>(round(mantissa));

    auto v = vbyte(13);
    wr<uint8_t>(v, 0, ids::real);
    wr<int64_t>(v, 1, m);
    wr<int32_t>(v, 9, exponent);
    return write(v) == v.size();
  }

  static auto write_bytes(fn_write write, span_cbyte value) noexcept
      -> bool {
    auto v = vbyte(9 + value.size());
    wr<uint8_t>(v, 0, ids::bytes);
    wr<uint64_t>(v, 1, value.size());
    serial::write_bytes({ v.begin() + 9, v.end() }, value);
    return write(v) == v.size();
  }

  static auto write_bitfield(fn_write    write,
                             cref_family value) noexcept -> bool {

    if (!value.is_vector()) {
      error_("Invalid value type.", __FUNCTION__);
      return false;
    }

    auto size  = value.get_size();
    auto bytes = vbyte((size + 7) / 8, 0);

    auto i = sl::index {};
    auto n = sl::index {};
    auto k = sl::index {};

    for (i = 0, n = 0; i < size; n++) {
      for (k = 0; k < 8 && i < size; k++, i++) {
        if (!value[i].is_boolean()) {
          error_("Invalid value.", __FUNCTION__);
          return false;
        }

        if (value[i].get_boolean()) {
          bytes[n] |= 0x80 >> k;
        }
      }
    }

    auto v = vbyte(9 + bytes.size());
    wr<uint8_t>(v, 0, ids::bitfield);
    wr<uint64_t>(v, 1, size);
    serial::write_bytes({ v.begin() + 9, v.end() },
                        span_cbyte { bytes });
    return write(v) == v.size();
  }

  static auto writedown(fn_write write, cref_family data) noexcept
      -> bool;

  static auto write_vector(fn_write write, cref_family value) noexcept
      -> bool {
    auto is_bitfield = [](cref_family value) -> bool {
      for (sl::index i = 0; i < value.get_size(); i++)
        if (!value[i].is_boolean()) {
          return false;
        }

      return true;
    };

    if (!value.is_vector()) {
      error_("Invalid value.", __FUNCTION__);
      return false;
    }

    if (is_bitfield(value)) {
      return write_bitfield(write, value);
    }

    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::vector);
    wr<uint64_t>(v, 1, value.get_size());

    if (write(v) != v.size())
      return false;

    for (sl::index i = 0; i < value.get_size(); i++) {
      if (!writedown(write, value[i]))
        return false;
    }

    return true;
  }

  static auto write_compact_composite(fn_write    write,
                                      cref_family value) noexcept
      -> bool {
    if (!value.is_composite()) {
      error_("Invalid value.", __FUNCTION__);
      return false;
    }

    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::compact_composite);
    wr<uint64_t>(v, 1, value.get_size());

    if (write(v) != v.size()) {
      return false;
    }

    v.resize(8);

    for (sl::index i = 0; i < value.get_size(); i++) {
      auto &key = value.get_key(i);

      if (!key.is_integer()) {
        error_("Invalid value.", __FUNCTION__);
        return false;
      }

      wr<int64_t>(v, 0, key.get_integer());

      if (write(v) != v.size())
        return false;
      if (!writedown(write, value[key]))
        return false;
    }

    return true;
  }

  static auto write_packed_composite(fn_write    write,
                                     cref_family value) noexcept
      -> bool {
    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::packed_composite);
    wr<uint64_t>(v, 1, value.get_size());

    if (write(v) != v.size()) {
      return false;
    }

    for (sl::index i = 0; i < value.get_size(); i++) {
      auto &key = value.get_key(i);

      if (!key.is_string()) {
        error_("Invalid key type.", __FUNCTION__);
        return false;
      }

      auto j = lower_bound(dictionary.begin(), dictionary.end(),
                           key.get_string());

      if (j == dictionary.end() || *j != key.get_string()) {
        error_("Invalid key.", __FUNCTION__);
        return false;
      }

      v.resize(8);
      wr<int64_t>(v, 0, j - dictionary.begin());

      if (write(v) != v.size())
        return false;
      if (!writedown(write, value.by_index(i)))
        return false;
    }

    return true;
  }

  static auto write_composite(fn_write    write,
                              cref_family value) noexcept -> bool {
    if (!value.is_composite()) {
      error_("Invalid value.", __FUNCTION__);
      return false;
    }

    if (is_packable(value)) {
      return write_packed_composite(write, value);
    }

    auto is_compact = [](cref_family value) -> bool {
      for (sl::index i = 0; i < value.get_size(); i++)
        if (!value.get_key(i).is_integer()) {
          return false;
        }

      return true;
    };

    if (is_compact(value)) {
      return write_compact_composite(write, value);
    }

    auto v = vbyte(9);
    wr<uint8_t>(v, 0, ids::composite);
    wr<uint64_t>(v, 1, value.get_size());

    if (write(v) != v.size()) {
      return false;
    }

    for (sl::index i = 0; i < value.get_size(); i++) {
      if (!writedown(write, value.get_key(i)))
        return false;
      if (!writedown(write, value.by_index(i)))
        return false;
    }

    return true;
  }

  static auto writedown(fn_write write, cref_family data) noexcept
      -> bool {
    if (data.is_empty())
      return write_empty(write);
    if (data.is_boolean())
      return write_bool(write, data.get_boolean());
    if (data.is_integer())
      return write_int(write, data.get_integer());
    if (data.is_string())
      return write_string(write, data.get_string());
    if (data.is_uint())
      return write_uint(write, data.get_uint());
    if (data.is_real())
      return write_real(write, data.get_real());
    if (data.is_bytes())
      return write_bytes(write, data.get_bytes());

    if (data.is_vector())
      return write_vector(write, data);
    if (data.is_composite())
      return write_composite(write, data);

    return false;
  }

  auto encode(fn_write write, const_pack_type data) noexcept -> bool {
    if (write) {
      auto v = vbyte(8);
      wr<uint64_t>(v, 0, magic);
      return write(v) == 8 && writedown(write, data);
    }

    return false;
  }
}
