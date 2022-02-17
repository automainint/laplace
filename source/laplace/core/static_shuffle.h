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
  class static_shuffle {
  public:
    static_assert(begin_ < end_);

    static_shuffle(static_shuffle const &) noexcept = default;
    static_shuffle(static_shuffle &&) noexcept      = default;
    static_shuffle &operator=(static_shuffle const &) noexcept =
        default;
    static_shuffle &operator=(static_shuffle &&) noexcept = default;

    explicit constexpr static_shuffle(uint64_t seed) noexcept;
    ~static_shuffle() noexcept = default;

    [[nodiscard]] constexpr auto operator[](uint64_t n) const noexcept
        -> uint64_t;

    [[nodiscard]] constexpr auto upper(uint64_t n) const noexcept
        -> uint64_t;

  private:
    uint64_t m_values[end_ - begin_] = {};
  };
}

#include "static_shuffle.impl.h"

#endif
