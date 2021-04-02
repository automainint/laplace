/*  laplace/core/c_family.cpp
 *
 *      Indexed family implementation.
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

#include "family.h"
#include "utils.h"
#include <algorithm>

namespace laplace::core {
  using std::min, std::move, std::lower_bound, std::string_view,
      std::wstring_view, std::u8string_view, std::u16string_view,
      std::u32string_view, std::numeric_limits, std::pair,
      std::vector, std::monostate, std::u8string;

  family::family(cref_vfamily value) {
    assign(value);
  }

  family::family(cref_composite value) {
    assign(value);
  }

  family::family(bool value) {
    assign(value);
  }

  family::family(int8_t value) {
    assign(static_cast<int64_t>(value));
  }

  family::family(int16_t value) {
    assign(static_cast<int64_t>(value));
  }

  family::family(int32_t value) {
    assign(static_cast<int64_t>(value));
  }

  family::family(int64_t value) {
    assign(static_cast<int64_t>(value));
  }

  family::family(uint8_t value) {
    assign(static_cast<uint64_t>(value));
  }

  family::family(uint16_t value) {
    assign(static_cast<uint64_t>(value));
  }

  family::family(uint32_t value) {
    assign(static_cast<uint64_t>(value));
  }

  family::family(uint64_t value) {
    assign(static_cast<uint64_t>(value));
  }

  family::family(float value) {
    assign(static_cast<double>(value));
  }

  family::family(double value) {
    assign(value);
  }

  family::family(string_view value) {
    assign(value);
  }

  family::family(wstring_view value) {
    assign(value);
  }

  family::family(u8string_view value) {
    assign(value);
  }

  family::family(u16string_view value) {
    assign(value);
  }

  family::family(u32string_view value) {
    assign(value);
  }

  family::family(const char *value) {
    assign(string_view(value));
  }

  family::family(const wchar_t *value) {
    assign(wstring_view(value));
  }

  family::family(const char8_t *value) {
    assign(u8string_view(value));
  }

  family::family(const char16_t *value) {
    assign(u16string_view(value));
  }

  family::family(const char32_t *value) {
    assign(u32string_view(value));
  }

  family::family(span_cbyte value) {
    assign(value);
  }

  auto family::operator=(family::cref_vfamily value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(family::cref_composite value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(bool value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(int8_t value) -> ref_family {
    assign(static_cast<int64_t>(value));
    return *this;
  }

  auto family::operator=(int16_t value) -> ref_family {
    assign(static_cast<int64_t>(value));
    return *this;
  }

  auto family::operator=(int32_t value) -> ref_family {
    assign(static_cast<int64_t>(value));
    return *this;
  }

  auto family::operator=(int64_t value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(uint8_t value) -> ref_family {
    assign(static_cast<uint64_t>(value));
    return *this;
  }

  auto family::operator=(uint16_t value) -> ref_family {
    assign(static_cast<uint64_t>(value));
    return *this;
  }

  auto family::operator=(uint32_t value) -> ref_family {
    assign(static_cast<uint64_t>(value));
    return *this;
  }

  auto family::operator=(uint64_t value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(float value) -> ref_family {
    assign(static_cast<double>(value));
    return *this;
  }

  auto family::operator=(double value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(string_view value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(wstring_view value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u8string_view value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u16string_view value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u32string_view value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(const char *value) -> ref_family {
    assign(string_view(value));
    return *this;
  }

  auto family::operator=(const wchar_t *value) -> ref_family {
    assign(wstring_view(value));
    return *this;
  }

  auto family::operator=(const char8_t *value) -> ref_family {
    assign(u8string_view(value));
    return *this;
  }

  auto family::operator=(const char16_t *value) -> ref_family {
    assign(u16string_view(value));
    return *this;
  }

  auto family::operator=(const char32_t *value) -> ref_family {
    assign(u32string_view(value));
    return *this;
  }

  auto family::operator=(span_cbyte value) -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator==(cref_family value) const noexcept -> bool {
    return compare(value) == 0;
  }

  auto family::operator!=(cref_family value) const noexcept -> bool {
    return compare(value) != 0;
  }

  auto family::operator<(cref_family value) const noexcept -> bool {
    return compare(value) < 0;
  }

  auto family::operator>(cref_family value) const noexcept -> bool {
    return compare(value) > 0;
  }

  auto family::operator<=(cref_family value) const noexcept -> bool {
    return compare(value) <= 0;
  }

  auto family::operator>=(cref_family value) const noexcept -> bool {
    return compare(value) >= 0;
  }

  family::operator bool() const {
    return get_boolean();
  }

  family::operator int64_t() const {
    return get_integer();
  }

  family::operator double() const {
    return get_real();
  }

  family::operator uint64_t() const {
    return get_uint();
  }

  family::operator u8string_view() const {
    return get_string();
  }

  auto family::operator[](size_t index) -> ref_family {
    return value(index);
  }

  auto family::operator[](size_t index) const -> cref_family {
    return get_value(index);
  }

  auto family::operator[](cref_family key) -> ref_family {
    return value(key);
  }

  auto family::operator[](cref_family key) const -> cref_family {
    return get_value(key);
  }

  auto family::is_empty() const -> bool {
    return m_data.index() == 0;
  }

  auto family::is_boolean() const -> bool {
    return m_data.index() == n_bool;
  }

  auto family::is_integer() const -> bool {
    return m_data.index() == n_int;
  }

  auto family::is_uint() const -> bool {
    return m_data.index() == n_uint;
  }

  auto family::is_real() const -> bool {
    return m_data.index() == n_real;
  }

  auto family::is_string() const -> bool {
    return m_data.index() == n_string;
  }

  auto family::is_bytes() const -> bool {
    return m_data.index() == n_bytes;
  }

  auto family::is_vector() const -> bool {
    return m_data.index() == n_vector;
  }

  auto family::is_composite() const -> bool {
    return m_data.index() == n_composite;
  }

  auto family::get_boolean() const -> bool {
    return m_data.index() == n_bool ? get<n_bool>(m_data) : false;
  }

  auto family::get_integer() const -> int64_t {
    constexpr auto max_int = static_cast<uint64_t>(
        numeric_limits<int64_t>::max());

    if (m_data.index() == n_uint && get<n_uint>(m_data) <= max_int) {
      return static_cast<int64_t>(get<n_uint>(m_data));
    }

    return m_data.index() == n_int ? get<n_int>(m_data) : 0ll;
  }

  auto family::get_real() const -> double {
    return m_data.index() == n_real ? get<n_real>(m_data) : .0;
  }

  auto family::get_string() const -> u8string_view {
    static const auto nil = u8string(u8"");

    return m_data.index() == n_string ? get<n_string>(m_data) : nil;
  }

  auto family::get_uint() const -> uint64_t {
    if (m_data.index() == n_int && get<n_int>(m_data) >= 0) {
      return static_cast<uint64_t>(get<n_int>(m_data));
    }

    return m_data.index() == n_uint ? get<n_uint>(m_data) : 0ull;
  }

  auto family::get_bytes() const -> span_cbyte {
    static const vbyte nil;

    return m_data.index() == n_bytes ? get<n_bytes>(m_data) : nil;
  }

  auto family::get_size() const -> size_t {
    if (m_data.index() == n_bytes) {
      return get<n_bytes>(m_data).size();
    }

    if (m_data.index() == n_vector) {
      return get<n_vector>(m_data).size();
    }

    if (m_data.index() == n_composite) {
      return get<n_composite>(m_data).size();
    }

    if (m_data.index() == n_string) {
      return get<n_string>(m_data).size();
    }

    return 0;
  }

  auto family::has(cref_family key) const -> bool {
    if (m_data.index() != n_composite) {
      return false;
    }

    bool result = true;

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), key,
                         op_lower_bound);

    if (i == get<n_composite>(m_data).end()) {
      result = false;
    } else if (i->first != key) {
      result = false;
    }

    return result;
  }

  void family::set_key(size_t index, cref_family k) {
    if (m_data.index() != n_composite)
      return;
    if (get<n_composite>(m_data).size() <= index)
      return;

    get<n_composite>(m_data).erase(        //
        get<n_composite>(m_data).begin() + //
        static_cast<ptrdiff_t>(index));

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), k,
                         op_lower_bound);

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, family() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, family() });
    }
  }

  void family::key(cref_family k) {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
    }

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), k,
                         op_lower_bound);

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, family() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, family() });
    }
  }

  auto family::value(size_t index) -> ref_family {
    if (m_data.index() != n_vector) {
      m_data = vector<family>();
    }

    if (get<n_vector>(m_data).size() <= index) {
      get<n_vector>(m_data).resize(index + 1);
    }

    return get<n_vector>(m_data)[index];
  }

  auto family::value(cref_family key) -> ref_family {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
    }

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), key,
                         op_lower_bound);

    if (i == get<n_composite>(m_data).end() || i->first != key) {
      return get<n_composite>(m_data)
          .emplace(i, pair { key, family() })
          ->second;
    }

    return i->second;
  }

  auto family::get_key(size_t index) const -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    if (get<n_composite>(m_data).size() <= index) {
      return out_of_range();
    }

    return get<n_composite>(m_data)[index].first;
  }

  auto family::get_value(size_t index) const -> cref_family {
    if (m_data.index() != n_vector) {
      return logic_error();
    }

    if (get<n_vector>(m_data).size() <= index) {
      return out_of_range();
    }

    return get<n_vector>(m_data)[index];
  }

  auto family::get_value(cref_family key) const -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), key,
                         op_lower_bound);

    if (i == get<n_composite>(m_data).end()) {
      return out_of_range();
    } else if (i->first != key) {
      return out_of_range();
    }

    return i->second;
  }

  auto family::by_key(size_t key) -> ref_family {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
    }

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), key,
                         op_lower_bound_uint);

    if (i == get<n_composite>(m_data).end()) {
      return get<n_composite>(m_data)
          .emplace_back(pair { key, family() })
          .second;
    } else if (!i->first.is_uint() || i->first.get_uint() != key) {
      return get<n_composite>(m_data)
          .emplace(i, pair { key, family() })
          ->second;
    } else {
      return i->second;
    }
  }

  auto family::by_key(size_t key) const -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(get<n_composite>(m_data).begin(),
                         get<n_composite>(m_data).end(), key,
                         op_lower_bound_uint);

    if (i == get<n_composite>(m_data).end()) {
      return out_of_range();
    } else if (!i->first.is_uint() || i->first.get_uint() != key) {
      return out_of_range();
    }

    return i->second;
  }

  auto family::compare(cref_family value) const noexcept -> int {
    if (m_data.index() != value.m_data.index()) {
      if (m_data.index() < value.m_data.index())
        return -1;
      return 1;
    }

    if (m_data.index() == n_uint) {
      try {
        if (get<n_uint>(m_data) == get<n_uint>(value.m_data))
          return 0;
        else if (get<n_uint>(m_data) < get<n_uint>(value.m_data))
          return -1;
        return 1;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_int) {
      try {
        if (get<n_int>(m_data) == get<n_int>(value.m_data))
          return 0;
        else if (get<n_int>(m_data) < get<n_int>(value.m_data))
          return -1;
        return 1;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_bool) {
      try {
        if (get<n_bool>(m_data) == get<n_bool>(value.m_data))
          return 0;
        else if (get<n_bool>(m_data) == true)
          return 1;
        return -1;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_real) {
      try {
        if (get<n_real>(m_data) == get<n_real>(value.m_data))
          return 0;
        else if (get<n_real>(m_data) < get<n_real>(value.m_data))
          return -1;
        return 1;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_string) {
      try {
        return get<n_string>(m_data).compare(
            get<n_string>(value.m_data));
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_bytes) {
      try {
        size_t na = get<n_bytes>(m_data).size();
        size_t nb = get<n_bytes>(value.m_data).size();

        size_t n = min(na, nb);

        for (size_t i = 0; i < n; i++) {
          if (get<n_bytes>(m_data)[i] < get<n_bytes>(value.m_data)[i])
            return -1;
          if (get<n_bytes>(m_data)[i] > get<n_bytes>(value.m_data)[i])
            return 1;
        }

        if (na < nb)
          return -1;
        if (na > nb)
          return 1;
        return 0;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_vector) {
      try {
        size_t na = get<n_vector>(m_data).size();
        size_t nb = get<n_vector>(value.m_data).size();

        size_t n = min(na, nb);

        for (size_t i = 0; i < n; i++) {
          auto x = get<n_vector>(m_data)[i].compare(
              get<n_vector>(value.m_data)[i]);

          if (x < 0)
            return -1;
          if (x > 0)
            return 1;
        }

        if (na < nb)
          return -1;
        if (na > nb)
          return 1;
        return 0;
      } catch (...) { _unreachable(); }
    }

    if (m_data.index() == n_composite) {
      try {
        size_t na = get<n_composite>(m_data).size();
        size_t nb = get<n_composite>(value.m_data).size();

        size_t n = min(na, nb);

        for (size_t i = 0; i < n; i++) {
          auto x = get<n_composite>(m_data)[i].first.compare(
              get<n_composite>(value.m_data)[i].first);

          if (x < 0)
            return -1;
          if (x > 0)
            return 1;

          x = get<n_composite>(m_data)[i].second.compare(
              get<n_composite>(value.m_data)[i].second);

          if (x < 0)
            return -1;
          if (x > 0)
            return 1;
        }

        if (na < nb)
          return -1;
        if (na > nb)
          return 1;
        return 0;
      } catch (...) { _unreachable(); }
    }

    return 0;
  }

  auto family::merge(cref_family f) -> bool {
    bool result = true;

    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
      result = false;
    }

    if (f.is_composite()) {
      for (size_t i = 0; i < f.get_size(); i++) {
        auto &k = f.get_key(i);

        if (has(k)) {
          if (get_value(k).is_composite() &&
              f.get_value(k).is_composite()) {
            result &= value(k).merge(f.get_value(k));
          } else if (value(k) != f.get_value(k)) {
            value(k) = f.get_value(k);
            result   = false;
          }
        } else {
          value(k) = f.get_value(k);
        }
      }
    }

    return result;
  }

  auto family::logic_error() -> cref_family {
    static const family result;
    return result;
  }

  auto family::out_of_range() -> cref_family {
    static const family result;
    return result;
  }

  void family::assign(family::cref_vfamily value) {
    if (value.size() > 0) {
      m_data = value;
    } else {
      m_data = monostate();
    }
  }

  void family::assign(family::cref_composite value) {
    if (value.size() > 0) {
      m_data = value;

      sort(get<n_composite>(m_data).begin(),
           get<n_composite>(m_data).end(), op_sort);
    } else {
      m_data = monostate();
    }
  }

  void family::assign(bool value) {
    m_data = value;
  }

  void family::assign(int64_t value) {
    m_data = value;
  }

  void family::assign(uint64_t value) {
    m_data = value;
  }

  void family::assign(double value) {
    m_data = value;
  }

  void family::assign(string_view value) {
    m_data = move(to_u8string(value));
  }

  void family::assign(wstring_view value) {
    m_data = move(to_u8string(value));
  }

  void family::assign(u8string_view value) {
    m_data = move(u8string(value));
  }

  void family::assign(u16string_view value) {
    m_data = move(to_u8string(value));
  }

  void family::assign(u32string_view value) {
    m_data = move(to_u8string(value));
  }

  void family::assign(span_cbyte value) {
    m_data = move(vbyte(value.begin(), value.end()));
  }

  auto family::op_lower_bound(const pair<family, family> &a,
                              const family &key) -> bool {
    return a.first < key;
  }

  auto family::op_lower_bound_uint(const pair<family, family> &a,
                                   size_t key) -> bool {
    return a.first.is_uint() ? a.first.get_uint() < key
                             : a.first.m_data.index() < n_uint;
  }

  auto family::op_sort(const pair<family, family> &a,
                       const pair<family, family> &b) -> bool {
    return a.first < b.first;
  }
}
