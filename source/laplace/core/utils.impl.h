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
  constexpr auto as_index(uint64_t value) noexcept -> size_t {
    if constexpr (sizeof(uint64_t) != sizeof(size_t)) {
      if (value > std::numeric_limits<size_t>::max()) {
        return -1;
      }
    }

    return static_cast<size_t>(value);
  }

  constexpr auto as_index(int64_t value) noexcept -> size_t {
    return as_index(static_cast<uint64_t>(value));
  }
}

#endif
