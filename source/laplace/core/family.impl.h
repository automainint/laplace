/*  laplace/core/family.impl.h
 *
 *      Indexed family inline methods.
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

#ifndef laplace_core_family_impl_h
#define laplace_core_family_impl_h

namespace laplace::core {
  inline void family::set_key(signed int n, cref_family k) noexcept {
    this->set_key(static_cast<signed long long>(n), k);
  }

  inline auto family::value(signed int n) noexcept -> ref_family {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto family::value(unsigned int n) noexcept -> ref_family {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto family::value(unsigned long long n) noexcept
      -> ref_family {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto family::value(const char *key) noexcept -> ref_family {
    return this->value(family(key));
  }

  inline auto family::value(const char8_t *key) noexcept -> ref_family {
    return this->value(family(key));
  }

  inline auto family::get_key(signed int n) const noexcept
      -> cref_family {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto family::get_value(signed int n) const noexcept
      -> cref_family {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto family::get_value(unsigned int n) const noexcept
      -> cref_family {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto family::get_value(unsigned long long n) const noexcept
      -> cref_family {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto family::get_value(const char *key) const noexcept
      -> cref_family {
    return this->get_value(family(key));
  }

  inline auto family::get_value(const char8_t *key) const noexcept
      -> cref_family {
    return this->get_value(family(key));
  }

  inline auto family::by_key(signed int key) noexcept -> ref_family {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto family::by_key(signed int key) const noexcept
      -> cref_family {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline family::operator float() const noexcept {
    return static_cast<float>(this->get_real());
  }

  inline family::operator signed int() const noexcept {
    return static_cast<signed int>(this->get_integer());
  }

  inline family::operator unsigned int() const noexcept {
    return static_cast<unsigned int>(this->get_uint());
  }

  inline auto family::operator[](signed int index) noexcept
      -> ref_family {
    return this->value(index);
  }

  inline auto family::operator[](signed int index) const noexcept
      -> cref_family {
    return this->get_value(index);
  }

  inline auto family::operator[](unsigned int n) noexcept
      -> ref_family {
    return this->value(n);
  }

  inline auto family::operator[](unsigned int n) const noexcept
      -> cref_family {
    return this->get_value(n);
  }

  inline auto family::operator[](unsigned long long n) noexcept
      -> ref_family {
    return this->value(n);
  }

  inline auto family::operator[](unsigned long long n) const noexcept
      -> cref_family {
    return this->get_value(n);
  }

  inline auto family::operator[](const char *key) noexcept
      -> ref_family {
    return this->value(key);
  }

  inline auto family::operator[](const char *key) const noexcept
      -> cref_family {
    return this->get_value(key);
  }

  inline auto family::operator[](const char8_t *key) noexcept
      -> ref_family {
    return this->value(key);
  }

  inline auto family::operator[](const char8_t *key) const noexcept
      -> cref_family {
    return this->get_value(key);
  }
}

#endif
