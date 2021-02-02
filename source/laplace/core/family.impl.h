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

#ifndef __laplace__core_family_impl__
#define __laplace__core_family_impl__

namespace laplace::core {
  inline auto family::value(int index) -> ref_family {
    return this->value(static_cast<size_t>(index));
  }

  inline auto family::value(const char *key) -> ref_family {
    return this->value(family(key));
  }

  inline auto family::get_value(int index) const
      -> cref_family {
    return this->get_value(static_cast<size_t>(index));
  }

  inline auto family::get_value(const char *key) const
      -> cref_family {
    return this->get_value(family(key));
  }

  inline family::operator float() const {
    return static_cast<float>(this->get_real());
  }

  inline family::operator int32_t() const {
    return static_cast<int32_t>(this->get_integer());
  }

  inline family::operator uint32_t() const {
    return static_cast<uint32_t>(this->get_uint());
  }

  inline auto family::operator[](int index) -> ref_family {
    return this->value(index);
  }

  inline auto family::operator[](int index) const
      -> cref_family {
    return this->get_value(index);
  }

  inline auto family::operator[](const char *key)
      -> ref_family {
    return this->value(key);
  }

  inline auto family::operator[](const char *key) const
      -> cref_family {
    return this->get_value(key);
  }

  inline auto family::operator[](const char8_t *key)
      -> ref_family {
    return this->value(key);
  }

  inline auto family::operator[](const char8_t *key) const
      -> cref_family {
    return this->get_value(key);
  }
}

#endif
