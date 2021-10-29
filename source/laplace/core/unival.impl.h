/*  laplace/core/unival.impl.h
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

#ifndef laplace_core_unival_impl_h
#define laplace_core_unival_impl_h

namespace laplace::core {
  inline auto unival::value(signed int n) noexcept -> ref_unival {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto unival::value(signed long n) noexcept -> ref_unival {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto unival::value(unsigned int n) noexcept -> ref_unival {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto unival::value(unsigned long n) noexcept -> ref_unival {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto unival::value(unsigned long long n) noexcept
      -> ref_unival {
    return this->value(static_cast<signed long long>(n));
  }

  inline auto unival::value(const char *key) noexcept -> ref_unival {
    return this->value(unival(key));
  }

  inline auto unival::value(const char8_t *key) noexcept
      -> ref_unival {
    return this->value(unival(key));
  }

  inline auto unival::get_key(signed int n) const noexcept
      -> cref_unival {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto unival::get_key(signed long n) const noexcept
      -> cref_unival {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto unival::get_key(unsigned int n) const noexcept
      -> cref_unival {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto unival::get_key(unsigned long n) const noexcept
      -> cref_unival {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto unival::get_key(unsigned long long n) const noexcept
      -> cref_unival {
    return this->get_key(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(signed int n) const noexcept
      -> cref_unival {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(signed long n) const noexcept
      -> cref_unival {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(unsigned int n) const noexcept
      -> cref_unival {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(unsigned long n) const noexcept
      -> cref_unival {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(unsigned long long n) const noexcept
      -> cref_unival {
    return this->get_value(static_cast<signed long long>(n));
  }

  inline auto unival::get_value(const char *key) const noexcept
      -> cref_unival {
    return this->get_value(unival(key));
  }

  inline auto unival::get_value(const char8_t *key) const noexcept
      -> cref_unival {
    return this->get_value(unival(key));
  }

  inline auto unival::by_key(signed int key) noexcept -> ref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(signed long key) noexcept -> ref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned int key) noexcept
      -> ref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned long key) noexcept
      -> ref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned long long key) noexcept
      -> ref_unival {
    return this->value(unival(key));
  }

  inline auto unival::by_key(signed int key) const noexcept
      -> cref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(signed long key) const noexcept
      -> cref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned int key) const noexcept
      -> cref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned long key) const noexcept
      -> cref_unival {
    return this->by_key(static_cast<signed long long>(key));
  }

  inline auto unival::by_key(unsigned long long key) const noexcept
      -> cref_unival {
    return this->get_value(unival(key));
  }

  inline auto unival::by_index(signed int key) noexcept
      -> ref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(signed long key) noexcept
      -> ref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned int key) noexcept
      -> ref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned long key) noexcept
      -> ref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned long long key) noexcept
      -> ref_unival {
    return this->value(unival(key));
  }

  inline auto unival::by_index(signed int key) const noexcept
      -> cref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(signed long key) const noexcept
      -> cref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned int key) const noexcept
      -> cref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned long key) const noexcept
      -> cref_unival {
    return this->by_index(static_cast<signed long long>(key));
  }

  inline auto unival::by_index(unsigned long long key) const noexcept
      -> cref_unival {
    return this->get_value(unival(key));
  }

  inline unival::operator signed char() const noexcept {
    return static_cast<signed char>(this->get_integer());
  }

  inline unival::operator signed short() const noexcept {
    return static_cast<signed short>(this->get_integer());
  }

  inline unival::operator signed int() const noexcept {
    return static_cast<signed int>(this->get_integer());
  }

  inline unival::operator signed long() const noexcept {
    return static_cast<signed long>(this->get_integer());
  }

  inline unival::operator unsigned char() const noexcept {
    return static_cast<unsigned char>(this->get_uint());
  }

  inline unival::operator unsigned short() const noexcept {
    return static_cast<unsigned short>(this->get_uint());
  }

  inline unival::operator unsigned int() const noexcept {
    return static_cast<unsigned int>(this->get_uint());
  }

  inline unival::operator unsigned long() const noexcept {
    return static_cast<unsigned long>(this->get_uint());
  }

  inline unival::operator float() const noexcept {
    return static_cast<float>(this->get_real());
  }

  inline auto unival::operator[](signed int n) noexcept
      -> ref_unival {
    return this->value(n);
  }

  inline auto unival::operator[](signed int n) const noexcept
      -> cref_unival {
    return this->get_value(n);
  }

  inline auto unival::operator[](signed long n) noexcept
      -> ref_unival {
    return this->value(n);
  }

  inline auto unival::operator[](signed long n) const noexcept
      -> cref_unival {
    return this->get_value(n);
  }

  inline auto unival::operator[](unsigned int n) noexcept
      -> ref_unival {
    return this->value(n);
  }

  inline auto unival::operator[](unsigned int n) const noexcept
      -> cref_unival {
    return this->get_value(n);
  }

  inline auto unival::operator[](unsigned long n) noexcept
      -> ref_unival {
    return this->value(n);
  }

  inline auto unival::operator[](unsigned long n) const noexcept
      -> cref_unival {
    return this->get_value(n);
  }

  inline auto unival::operator[](unsigned long long n) noexcept
      -> ref_unival {
    return this->value(n);
  }

  inline auto unival::operator[](unsigned long long n) const noexcept
      -> cref_unival {
    return this->get_value(n);
  }

  inline auto unival::operator[](const char *key) noexcept
      -> ref_unival {
    return this->value(key);
  }

  inline auto unival::operator[](const char *key) const noexcept
      -> cref_unival {
    return this->get_value(key);
  }

  inline auto unival::operator[](const char8_t *key) noexcept
      -> ref_unival {
    return this->value(key);
  }

  inline auto unival::operator[](const char8_t *key) const noexcept
      -> cref_unival {
    return this->get_value(key);
  }
}

#endif
