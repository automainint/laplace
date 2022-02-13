/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_STATIC_RANDOM_H
#define LAPLACE_CORE_STATIC_RANDOM_H

#include "defs.h"

namespace laplace::core {
  class static_random {
  public:
    static_random(static_random const &) noexcept = default;
    static_random(static_random &&) noexcept      = default;
    static_random &operator=(static_random const &) noexcept =
        default;
    static_random &operator=(static_random &&) noexcept = default;

    explicit constexpr static_random(uint64_t seed) noexcept;
    ~static_random() noexcept = default;

    [[nodiscard]] constexpr auto operator()() noexcept -> uint64_t;

  private:
    struct _pair {
      uint64_t x, z;
    };

    [[nodiscard]] static constexpr auto splitmix64(
        uint64_t x) noexcept -> _pair;
    [[nodiscard]] static constexpr auto rotl(uint64_t x,
                                             int      k) noexcept
        -> uint64_t;

    uint64_t m_state[2] = { 1, 2 };
  };
}

#include "static_random.impl.h"

#endif
