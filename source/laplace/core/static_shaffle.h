/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_STATIC_SHAFFLE_H
#define LAPLACE_CORE_STATIC_SHAFFLE_H

#include "static_random.h"

namespace laplace::core {
  template <uint64_t begin_, uint64_t end_>
  class static_shaffle {
  public:
    static_assert(begin_ < end_);

    static_shaffle(static_shaffle const &) noexcept = default;
    static_shaffle(static_shaffle &&) noexcept      = default;
    static_shaffle &operator=(static_shaffle const &) noexcept =
        default;
    static_shaffle &operator=(static_shaffle &&) noexcept = default;

    explicit constexpr static_shaffle(uint64_t seed) noexcept;
    ~static_shaffle() noexcept = default;

    [[nodiscard]] constexpr auto operator[](uint64_t n) const noexcept
        -> uint64_t;

    [[nodiscard]] constexpr auto upper(uint64_t n) const noexcept
        -> uint64_t;

  private:
    uint64_t m_values[end_ - begin_] = {};
  };
}

#include "static_shaffle.impl.h"

#endif
