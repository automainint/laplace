/*  laplace/core/c_unival.cpp
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

#include "unival.h"

#include "utils.h"
#include <algorithm>

namespace laplace::core {
  using std::min, std::lower_bound, std::string_view,
      std::wstring_view, std::u8string_view, std::u16string_view,
      std::u32string_view, std::numeric_limits, std::pair,
      std::vector, std::monostate, std::u8string;

  unival::unival(cref_vec value) noexcept {
    assign(value);
  }

  unival::unival(cref_composite value) noexcept {
    assign(value);
  }

  unival::unival(bool value) noexcept {
    assign(value);
  }

  unival::unival(signed char value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(signed short value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(signed int value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(signed long value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(signed long long value) noexcept {
    assign(value);
  }

  unival::unival(unsigned char value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(unsigned short value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(unsigned int value) noexcept {
    assign(static_cast<signed long long>(value));
  }

  unival::unival(unsigned long value) noexcept {
    assign(static_cast<unsigned long long>(value));
  }

  unival::unival(unsigned long long value) noexcept {
    assign(value);
  }

  unival::unival(float value) noexcept {
    assign(static_cast<double>(value));
  }

  unival::unival(double value) noexcept {
    assign(value);
  }

  unival::unival(string_view value) noexcept {
    assign(value);
  }

  unival::unival(wstring_view value) noexcept {
    assign(value);
  }

  unival::unival(u8string_view value) noexcept {
    assign(value);
  }

  unival::unival(u16string_view value) noexcept {
    assign(value);
  }

  unival::unival(u32string_view value) noexcept {
    assign(value);
  }

  unival::unival(const char *value) noexcept {
    assign(string_view(value));
  }

  unival::unival(const wchar_t *value) noexcept {
    assign(wstring_view(value));
  }

  unival::unival(const char8_t *value) noexcept {
    assign(u8string_view(value));
  }

  unival::unival(const char16_t *value) noexcept {
    assign(u16string_view(value));
  }

  unival::unival(const char32_t *value) noexcept {
    assign(u32string_view(value));
  }

  unival::unival(span_cbyte value) noexcept {
    assign(value);
  }

  auto unival::operator=(unival::cref_vec value) noexcept
      -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(unival::cref_composite value) noexcept
      -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(bool value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(signed char value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(signed short value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(signed int value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(signed long value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(signed long long value) noexcept
      -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(unsigned char value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(unsigned short value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(unsigned int value) noexcept -> unival & {
    assign(static_cast<signed long long>(value));
    return *this;
  }

  auto unival::operator=(unsigned long value) noexcept -> unival & {
    assign(static_cast<unsigned long long>(value));
    return *this;
  }

  auto unival::operator=(unsigned long long value) noexcept
      -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(float value) noexcept -> unival & {
    assign(static_cast<double>(value));
    return *this;
  }

  auto unival::operator=(double value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(string_view value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(wstring_view value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(u8string_view value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(u16string_view value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(u32string_view value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator=(const char *value) noexcept -> unival & {
    assign(string_view { value });
    return *this;
  }

  auto unival::operator=(const wchar_t *value) noexcept -> unival & {
    assign(wstring_view { value });
    return *this;
  }

  auto unival::operator=(const char8_t *value) noexcept -> unival & {
    assign(u8string_view { value });
    return *this;
  }

  auto unival::operator=(const char16_t *value) noexcept -> unival & {
    assign(u16string_view { value });
    return *this;
  }

  auto unival::operator=(const char32_t *value) noexcept -> unival & {
    assign(u32string_view { value });
    return *this;
  }

  auto unival::operator=(span_cbyte value) noexcept -> unival & {
    assign(value);
    return *this;
  }

  auto unival::operator==(cref_unival value) const noexcept -> bool {
    return compare(value) == 0;
  }

  auto unival::operator!=(cref_unival value) const noexcept -> bool {
    return compare(value) != 0;
  }

  auto unival::operator<(cref_unival value) const noexcept -> bool {
    return compare(value) < 0;
  }

  auto unival::operator>(cref_unival value) const noexcept -> bool {
    return compare(value) > 0;
  }

  auto unival::operator<=(cref_unival value) const noexcept -> bool {
    return compare(value) <= 0;
  }

  auto unival::operator>=(cref_unival value) const noexcept -> bool {
    return compare(value) >= 0;
  }

  unival::operator bool() const noexcept {
    return get_boolean();
  }

  unival::operator signed long long() const noexcept {
    return get_integer();
  }

  unival::operator unsigned long long() const noexcept {
    return get_uint();
  }

  unival::operator double() const noexcept {
    return get_real();
  }

  auto unival::operator[](signed long long n) noexcept -> ref_unival {
    return value(n);
  }

  auto unival::operator[](signed long long n) const noexcept
      -> cref_unival {
    return get_value(n);
  }

  auto unival::operator[](cref_unival key) noexcept -> ref_unival {
    return value(key);
  }

  auto unival::operator[](cref_unival key) const noexcept
      -> cref_unival {
    return get_value(key);
  }

  auto unival::is_empty() const noexcept -> bool {
    return m_data.index() == 0;
  }

  auto unival::is_boolean() const noexcept -> bool {
    return m_data.index() == n_bool;
  }

  auto unival::is_integer() const noexcept -> bool {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (m_data.index() == n_uint && get<n_uint>(m_data) <= max_int) {
      return true;
    }

    return m_data.index() == n_int;
  }

  auto unival::is_uint() const noexcept -> bool {
    if (m_data.index() == n_int && get<n_int>(m_data) >= 0) {
      return true;
    }

    return m_data.index() == n_uint;
  }

  auto unival::is_real() const noexcept -> bool {
    return m_data.index() == n_real;
  }

  auto unival::is_string() const noexcept -> bool {
    return m_data.index() == n_string;
  }

  auto unival::is_bytes() const noexcept -> bool {
    return m_data.index() == n_bytes;
  }

  auto unival::is_vector() const noexcept -> bool {
    return m_data.index() == n_vector;
  }

  auto unival::is_composite() const noexcept -> bool {
    return m_data.index() == n_composite;
  }

  auto unival::get_boolean() const noexcept -> bool {
    return m_data.index() == n_bool ? get<n_bool>(m_data) : false;
  }

  auto unival::get_integer() const noexcept -> signed long long {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (m_data.index() == n_uint && get<n_uint>(m_data) <= max_int) {
      return static_cast<signed long long>(get<n_uint>(m_data));
    }

    return m_data.index() == n_int ? get<n_int>(m_data) : 0ll;
  }

  auto unival::get_uint() const noexcept -> unsigned long long {
    if (m_data.index() == n_int && get<n_int>(m_data) >= 0) {
      return static_cast<unsigned long long>(get<n_int>(m_data));
    }

    return m_data.index() == n_uint ? get<n_uint>(m_data) : 0ull;
  }

  auto unival::get_real() const noexcept -> double {
    return m_data.index() == n_real ? get<n_real>(m_data) : .0;
  }

  auto unival::get_string() const noexcept -> u8string_view {
    static const auto nil = u8string(u8"");

    return m_data.index() == n_string ? get<n_string>(m_data) : nil;
  }

  auto unival::get_bytes() const noexcept -> span_cbyte {
    static const vbyte nil;

    return m_data.index() == n_bytes ? get<n_bytes>(m_data) : nil;
  }

  auto unival::get_size() const noexcept -> sl::whole {
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

  auto unival::has(cref_unival key) const noexcept -> bool {
    if (m_data.index() != n_composite) {
      return false;
    }

    bool result = true;

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<unival, unival> &x, const unival &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      result = false;
    } else if (i->first != key) {
      result = false;
    }

    return result;
  }

  void unival::set_key(signed long long n, cref_unival k) noexcept {
    if (m_data.index() != n_composite)
      return;
    if (get<n_composite>(m_data).size() <= n)
      return;

    get<n_composite>(m_data).erase(get<n_composite>(m_data).begin() +
                                   n);

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), k,
        [](const pair<unival, unival> &x, const unival &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, unival() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, unival() });
    }
  }

  void unival::key(cref_unival k) noexcept {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<unival, unival>>();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), k,
        [](const pair<unival, unival> &x, const unival &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      get<n_composite>(m_data).emplace_back(pair { k, unival() });
    } else if (i->first != k) {
      get<n_composite>(m_data).emplace(i, pair { k, unival() });
    }
  }

  auto unival::value(signed long long n) noexcept -> ref_unival {
    if (m_data.index() != n_vector) {
      m_data = vector<unival>();
    }

    if (n < 0) {
      n = get<n_vector>(m_data).size();
    }

    if (get<n_vector>(m_data).size() <= n) {
      get<n_vector>(m_data).resize(n + 1);
    }

    return get<n_vector>(m_data)[n];
  }

  auto unival::value(cref_unival key) noexcept -> ref_unival {
    if (m_data.index() != n_composite) {
      m_data = vector<pair<unival, unival>>();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<unival, unival> &x, const unival &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end() || i->first != key) {
      return get<n_composite>(m_data)
          .emplace(i, pair { key, unival() })
          ->second;
    }

    return i->second;
  }

  auto unival::get_key(signed long long n) const noexcept
      -> cref_unival {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    if (n < 0 || get<n_composite>(m_data).size() <= n) {
      return out_of_range();
    }

    return get<n_composite>(m_data)[n].first;
  }

  auto unival::get_value(signed long long n) const noexcept
      -> cref_unival {
    if (m_data.index() != n_vector) {
      return logic_error();
    }

    if (n < 0 || get<n_vector>(m_data).size() <= n) {
      return out_of_range();
    }

    return get<n_vector>(m_data)[n];
  }

  auto unival::get_value(cref_unival key) const noexcept
      -> cref_unival {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<unival, unival> &x, const unival &k) {
          return x.first.compare(k) < 0;
        });

    if (i == get<n_composite>(m_data).end()) {
      return out_of_range();
    } else if (i->first != key) {
      return out_of_range();
    }

    return i->second;
  }

  auto unival::by_key(signed long long key) noexcept -> ref_unival {
    if (m_data.index() != n_composite) {
      m_data = composite {};
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<unival, unival> &x, signed long long k) {
          if (x.first.m_data.index() != n_int) {
            return x.first.m_data.index() < n_int;
          }

          return get<n_int>(x.first.m_data) < k;
        });

    if (i == get<n_composite>(m_data).end()) {
      return get<n_composite>(m_data)
          .emplace_back(pair { key, unival() })
          .second;
    } else if (!i->first.is_uint() || i->first.get_uint() != key) {
      return get<n_composite>(m_data)
          .emplace(i, pair { key, unival() })
          ->second;
    } else {
      return i->second;
    }
  }

  auto unival::by_key(signed long long key) const noexcept
      -> cref_unival {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    auto i = lower_bound(
        get<n_composite>(m_data).begin(),
        get<n_composite>(m_data).end(), key,
        [](const pair<unival, unival> &x, signed long long k) {
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

  auto unival::by_index(signed long long n) noexcept -> ref_unival {
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

  auto unival::by_index(signed long long n) const noexcept
      -> cref_unival {
    if (m_data.index() != n_composite) {
      return logic_error();
    }

    if (n < 0 || n >= get<n_composite>(m_data).size()) {
      return out_of_range();
    }

    return get<n_composite>(m_data)[n].second;
  }

  auto unival::compare(cref_unival value) const noexcept -> int {
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

  auto unival::merge(cref_unival f) noexcept -> bool {
    bool result = true;

    if (m_data.index() != n_composite) {
      m_data = vector<pair<unival, unival>>();
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

  auto unival::logic_error() noexcept -> cref_unival {
    static const auto result = unival {};
    return result;
  }

  auto unival::out_of_range() noexcept -> cref_unival {
    static const auto result = unival {};
    return result;
  }

  void unival::assign(cref_vec value) noexcept {
    if (value.size() > 0) {
      m_data = value;
    } else {
      m_data = monostate();
    }
  }

  void unival::assign(cref_composite value) noexcept {
    if (value.size() > 0) {
      m_data = value;

      sort(get<n_composite>(m_data).begin(),
           get<n_composite>(m_data).end(),
           [](const pair<unival, unival> &a,
              const pair<unival, unival> &b) {
             return a.first.compare(b.first) < 0;
           });

    } else {
      m_data = monostate();
    }
  }

  void unival::assign(bool value) noexcept {
    m_data = value;
  }

  void unival::assign(signed long long value) noexcept {
    m_data = value;
  }

  void unival::assign(unsigned long long value) noexcept {
    constexpr auto max_int = static_cast<unsigned long long>(
        numeric_limits<signed long long>::max());

    if (value <= max_int) {
      m_data = static_cast<signed long long>(value);
    } else {
      m_data = value;
    }
  }

  void unival::assign(double value) noexcept {
    m_data = value;
  }

  void unival::assign(string_view value) noexcept {
    m_data = to_u8string(value);
  }

  void unival::assign(wstring_view value) noexcept {
    m_data = to_u8string(value);
  }

  void unival::assign(u8string_view value) noexcept {
    m_data = u8string { value };
  }

  void unival::assign(u16string_view value) noexcept {
    m_data = to_u8string(value);
  }

  void unival::assign(u32string_view value) noexcept {
    m_data = to_u8string(value);
  }

  void unival::assign(span_cbyte value) noexcept {
    m_data = vbyte { value.begin(), value.end() };
  }
}
