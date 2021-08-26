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

  family::family(cref_vfamily value) noexcept {
    assign(value);
  }

  family::family(cref_composite value) noexcept {
    assign(value);
  }

  family::family(bool value) noexcept {
    assign(value);
  }

  family::family(signed char value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(signed short value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(signed int value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(signed long value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(signed long long value) noexcept {
    assign(value);
  }

  family::family(unsigned char value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(unsigned short value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(unsigned int value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  family::family(unsigned long value) noexcept {
    assign(static_cast<unsigned long long>(value));
  }

  family::family(unsigned long long value) noexcept {
    assign(value);
  }

  family::family(float value) noexcept {
    assign(static_cast<double>(value));
  }

  family::family(double value) noexcept {
    assign(value);
  }

  family::family(string_view value) noexcept {
    assign(value);
  }

  family::family(wstring_view value) noexcept {
    assign(value);
  }

  family::family(u8string_view value) noexcept {
    assign(value);
  }

  family::family(u16string_view value) noexcept {
    assign(value);
  }

  family::family(u32string_view value) noexcept {
    assign(value);
  }

  family::family(const char *value) noexcept {
    assign(string_view(value));
  }

  family::family(const wchar_t *value) noexcept {
    assign(wstring_view(value));
  }

  family::family(const char8_t *value) noexcept {
    assign(u8string_view(value));
  }

  family::family(const char16_t *value) noexcept {
    assign(u16string_view(value));
  }

  family::family(const char32_t *value) noexcept {
    assign(u32string_view(value));
  }

  family::family(span_cbyte value) noexcept {
    assign(value);
  }

  auto family::operator=(family::cref_vfamily value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(family::cref_composite value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(bool value) noexcept -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(signed char value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(signed short value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(signed int value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(signed long value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(signed long long value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(unsigned char value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(unsigned short value) noexcept
      -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(unsigned int value) noexcept -> ref_family {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto family::operator=(unsigned long value) noexcept -> ref_family {
    assign(static_cast<unsigned long long>(value));
    return *this;
  }

  auto family::operator=(unsigned long long value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(float value) noexcept -> ref_family {
    assign(static_cast<double>(value));
    return *this;
  }

  auto family::operator=(double value) noexcept -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(string_view value) noexcept -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(wstring_view value) noexcept -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u8string_view value) noexcept -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u16string_view value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(u32string_view value) noexcept
      -> ref_family {
    assign(value);
    return *this;
  }

  auto family::operator=(const char *value) noexcept -> ref_family {
    assign(string_view { value });
    return *this;
  }

  auto family::operator=(const wchar_t *value) noexcept
      -> ref_family {
    assign(wstring_view { value });
    return *this;
  }

  auto family::operator=(const char8_t *value) noexcept
      -> ref_family {
    assign(u8string_view { value });
    return *this;
  }

  auto family::operator=(const char16_t *value) noexcept
      -> ref_family {
    assign(u16string_view { value });
    return *this;
  }

  auto family::operator=(const char32_t *value) noexcept
      -> ref_family {
    assign(u32string_view { value });
    return *this;
  }

  auto family::operator=(span_cbyte value) noexcept -> ref_family {
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

  family::operator bool() const noexcept {
    return get_boolean();
  }

  family::operator signed long long() const noexcept {
    return get_integer();
  }

  family::operator unsigned long long() const noexcept {
    return get_uint();
  }

  family::operator double() const noexcept {
    return get_real();
  }

  auto family::operator[](signed long long n) noexcept -> ref_family {
    return value(n);
  }

  auto family::operator[](signed long long n) const noexcept
      -> cref_family {
    return get_value(n);
  }

  auto family::operator[](cref_family key) noexcept -> ref_family {
    return value(key);
  }

  auto family::operator[](cref_family key) const noexcept
      -> cref_family {
    return get_value(key);
  }

  auto family::is_empty() const noexcept -> bool {
    return m_data.index() == 0;
  }

  auto family::is_boolean() const noexcept -> bool {
    return m_data.index() == n_bool;
  }

  auto family::is_integer() const noexcept -> bool {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (m_data.index() == n_uint && get<n_uint>(m_data) <= max_int) {
      return true;
    }

    return m_data.index() == n_int;
  }

  auto family::is_uint() const noexcept -> bool {
    if (m_data.index() == n_int && get<n_int>(m_data) >= 0) {
      return true;
    }

    return m_data.index() == n_uint;
  }

  auto family::is_real() const noexcept -> bool {
    return m_data.index() == n_real;
  }

  auto family::is_string() const noexcept -> bool {
    return m_data.index() == n_string;
  }

  auto family::is_bytes() const noexcept -> bool {
    return m_data.index() == n_bytes;
  }

  auto family::is_vector() const noexcept -> bool {
    return m_data.index() == n_vector;
  }

  auto family::is_composite() const noexcept -> bool {
    return m_data.index() == n_composite;
  }

  auto family::get_boolean() const noexcept -> bool {
    return m_data.index() == n_bool ? get<n_bool>(m_data) : false;
  }

  auto family::get_integer() const noexcept -> signed long long {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (m_data.index() == n_uint && get<n_uint>(m_data) <= max_int) {
      return static_cast<signed long long>(get<n_uint>(m_data));
    }

    return m_data.index() == n_int ? get<n_int>(m_data) : 0ll;
  }

  auto family::get_uint() const noexcept -> unsigned long long {
    if (m_data.index() == n_int && get<n_int>(m_data) >= 0) {
      return static_cast<unsigned long long>(get<n_int>(m_data));
    }

    return m_data.index() == n_uint ? get<n_uint>(m_data) : 0ull;
  }

  auto family::get_real() const noexcept -> double {
    return m_data.index() == n_real ? get<n_real>(m_data) : .0;
  }

  auto family::get_string() const noexcept -> u8string_view {
    static const auto nil = u8string(u8"");

    return m_data.index() == n_string ? get<n_string>(m_data) : nil;
  }

  auto family::get_bytes() const noexcept -> span_cbyte {
    static const vbyte nil;

    return m_data.index() == n_bytes ? get<n_bytes>(m_data) : nil;
  }

  auto family::get_size() const noexcept -> sl::whole {
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

  auto family::has(cref_family key) const noexcept -> bool {
    if (m_data.index() != n_composite) {
      return false;
    }

    bool result = true;

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<family, family> &x, const family &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      result = false;
    } else if (i->first != key) {
      result = false;
    }

    return result;
  }

  void family::set_key(signed long long n, cref_family k) noexcept {
    if (m_data.index() != n_composite)
      return;
    if (get<n_composite>(m_data).size() <= n)
      return;

    get<n_composite>(m_data).erase(get<n_composite>(m_data).begin() +
                                   n);

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), k,
        [](const pair<family, family> &x, const family &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, family() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, family() });
    }
  }

  void family::key(cref_family k) noexcept {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), k,
        [](const pair<family, family> &x, const family &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, family() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, family() });
    }
  }

  auto family::value(signed long long n) noexcept -> ref_family {
    if (m_data.index() != n_vector) {
      m_data = vector<family>();
    }

    if (n < 0) {
      n = get<n_vector>(m_data).size();
    }

    if (get<n_vector>(m_data).size() <= n) {
      get<n_vector>(m_data).resize(n + 1);
    }

    return get<n_vector>(m_data)[n];
  }

  auto family::value(cref_family key) noexcept -> ref_family {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<family, family> &x, const family &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end() || i->first != key) {
      return get<n_composite>(m_data)
          .emplace(i, pair { key, family() })
          ->second;
    }

    return i->second;
  }

  auto family::get_key(signed long long n) const noexcept
      -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    if (n < 0 || get<n_composite>(m_data).size() <= n) {
      return out_of_range();
    }

    return get<n_composite>(m_data)[n].first;
  }

  auto family::get_value(signed long long n) const noexcept
      -> cref_family {
    if (m_data.index() != n_vector) {
      return logic_error();
    }

    if (n < 0 || get<n_vector>(m_data).size() <= n) {
      return out_of_range();
    }

    return get<n_vector>(m_data)[n];
  }

  auto family::get_value(cref_family key) const noexcept
      -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<family, family> &x, const family &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      return out_of_range();
    } else if (i->first != key) {
      return out_of_range();
    }

    return i->second;
  }

  auto family::by_key(signed long long key) noexcept -> ref_family {
    if (m_data.index() != n_composite) {
      m_data = composite {};
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<family, family> &x, signed long long k) {
          if (x.first.m_data.index() != n_int) {
            return x.first.m_data.index() < n_int;
          }

          return get<n_int>(x.first.m_data) < k;
        });

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

  auto family::by_key(signed long long key) const noexcept
      -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<family, family> &x, signed long long k) {
          if (x.first.m_data.index() != n_int) {
            return x.first.m_data.index() < n_int;
          }

          return get<n_int>(x.first.m_data) < k;
        });

    if (i == get<n_composite>(m_data).end()) {
      return out_of_range();
    } else if (!i->first.is_uint() || i->first.get_uint() != key) {
      return out_of_range();
    }

    return i->second;
  }

  auto family::by_index(signed long long n) noexcept -> ref_family {
    if (m_data.index() != n_composite) {
      m_data = composite {};
    }

    if (n < 0) {
      n = get<n_composite>(m_data).size();
    }

    if (get<n_composite>(m_data).size() <= n) {
      get<n_composite>(m_data).resize(n + 1);
    }

    return get<n_composite>(m_data)[n].second;
  }

  auto family::by_index(signed long long n) const noexcept
      -> cref_family {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    if (n < 0 || n >= get<n_composite>(m_data).size()) {
      return out_of_range();
    }

    return get<n_composite>(m_data)[n].second;
  }

  auto family::compare(cref_family value) const noexcept -> int {
    if (m_data.index() != value.m_data.index()) {
      if (m_data.index() < value.m_data.index())
        return -1;
      return 1;
    }

    if (m_data.index() == n_uint) {
      if (get<n_uint>(m_data) == get<n_uint>(value.m_data))
        return 0;
      else if (get<n_uint>(m_data) < get<n_uint>(value.m_data))
        return -1;
      return 1;
    }

    if (m_data.index() == n_int) {
      if (get<n_int>(m_data) == get<n_int>(value.m_data))
        return 0;
      else if (get<n_int>(m_data) < get<n_int>(value.m_data))
        return -1;
      return 1;
    }

    if (m_data.index() == n_bool) {
      if (get<n_bool>(m_data) == get<n_bool>(value.m_data))
        return 0;
      else if (get<n_bool>(m_data) == true)
        return 1;
      return -1;
    }

    if (m_data.index() == n_real) {
      const auto a = get<n_real>(m_data);
      const auto b = get<n_real>(value.m_data);
      const auto x = a - b;

      constexpr auto eps = numeric_limits<double>::epsilon();

      if (x < eps && x > -eps)
        return 0;
      else if (a < b)
        return -1;
      return 1;
    }

    if (m_data.index() == n_string) {
      return get<n_string>(m_data).compare(
          get<n_string>(value.m_data));
    }

    if (m_data.index() == n_bytes) {
      auto na = get<n_bytes>(m_data).size();
      auto nb = get<n_bytes>(value.m_data).size();

      auto n = min(na, nb);

      for (sl::index i = 0; i < n; i++) {
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
    }

    if (m_data.index() == n_vector) {
      auto na = get<n_vector>(m_data).size();
      auto nb = get<n_vector>(value.m_data).size();

      auto n = min(na, nb);

      for (sl::index i = 0; i < n; i++) {
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
    }

    if (m_data.index() == n_composite) {
      auto na = get<n_composite>(m_data).size();
      auto nb = get<n_composite>(value.m_data).size();

      auto n = min(na, nb);

      for (sl::index i = 0; i < n; i++) {
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
    }

    return 0;
  }

  auto family::merge(cref_family f) noexcept -> bool {
    bool result = true;

    if (m_data.index() != n_composite) {
      m_data = vector<pair<family, family>>();
      result = false;
    }

    if (f.is_composite()) {
      for (sl::index i = 0; i < f.get_size(); i++) {
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

  auto family::logic_error() noexcept -> cref_family {
    static const auto result = family {};
    return result;
  }

  auto family::out_of_range() noexcept -> cref_family {
    static const auto result = family {};
    return result;
  }

  void family::assign(family::cref_vfamily value) noexcept {
    if (value.size() > 0) {
      m_data = value;
    } else {
      m_data = monostate();
    }
  }

  void family::assign(family::cref_composite value) noexcept {
    if (value.size() > 0) {
      m_data = value;

      sort(get<n_composite>(m_data).begin(),
           get<n_composite>(m_data).end(),
           [](const pair<family, family> &a,
              const pair<family, family> &b) {
             return a.first.compare(b.first) < 0;
           });

    } else {
      m_data = monostate();
    }
  }

  void family::assign(bool value) noexcept {
    m_data = value;
  }

  void family::assign(signed long long value) noexcept {
    m_data = value;
  }

  void family::assign(unsigned long long value) noexcept {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (value <= max_int) {
      verb("  assign as signed");
      m_data = static_cast<signed long long>(value);
    } else {
      verb("  assign as unsigned");
      m_data = value;
    }
  }

  void family::assign(double value) noexcept {
    m_data = value;
  }

  void family::assign(string_view value) noexcept {
    m_data = move(to_u8string(value));
  }

  void family::assign(wstring_view value) noexcept {
    m_data = move(to_u8string(value));
  }

  void family::assign(u8string_view value) noexcept {
    m_data = move(u8string(value));
  }

  void family::assign(u16string_view value) noexcept {
    m_data = move(to_u8string(value));
  }

  void family::assign(u32string_view value) noexcept {
    m_data = move(to_u8string(value));
  }

  void family::assign(span_cbyte value) noexcept {
    m_data = move(vbyte(value.begin(), value.end()));
  }
}
