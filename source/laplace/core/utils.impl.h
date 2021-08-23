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
  constexpr auto as_index(uint64_t value, sl::index invalid) noexcept
      -> sl::index {
    const auto n = static_cast<sl::index>(value);

    if constexpr (sizeof value != sizeof n) {
      if (static_cast<decltype(value)>(n) != value) {
        error_("32-bit index value overflow.", __FUNCTION__);
        return invalid;
      }
    }

    return n;
  }

  constexpr auto as_index(int64_t value, sl::index invalid) noexcept
      -> sl::index {
    const auto n = static_cast<sl::index>(value);

    if constexpr (sizeof value != sizeof n) {
      if (static_cast<decltype(value)>(n) != value) {
        error_("32-bit index value overflow.", __FUNCTION__);
        return invalid;
      }
    }

    return n;
  }
}

#endif
