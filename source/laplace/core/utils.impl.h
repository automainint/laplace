/*  laplace/core/utils.impl.h
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

#ifndef laplace_core_utils_impl_h
#define laplace_core_utils_impl_h

namespace laplace {
  constexpr auto as_index(const uint64_t  value,
                          const sl::index invalid) noexcept
      -> sl::index {

    if constexpr (!_unsafe) {
      if (value == -1) {
        return invalid;
      }

      if (value > std::numeric_limits<sl::index>::max()) {
        error_("Index value overflow.", __FUNCTION__);
        return invalid;
      }
    }

    return static_cast<sl::index>(value);
  }

  constexpr auto as_index(const int64_t   value,
                          const sl::index invalid) noexcept
      -> sl::index {

    if constexpr (!_unsafe) {
      if (value == -1) {
        return invalid;
      }

      if constexpr (sizeof value > sizeof(sl::index)) {
        if (value < 0 ||
            value > std::numeric_limits<sl::index>::max()) {
          error_("Index value overflow.", __FUNCTION__);
          return invalid;
        }
      } else {
        if (value < 0) {
          error_("Index value overflow.", __FUNCTION__);
          return invalid;
        }
      }
    }

    return static_cast<sl::index>(value);
  }
}

#endif
