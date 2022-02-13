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
  class static_shaffle {
  public:
    struct _init {
      uint64_t begin;
      uint64_t end;
      uint64_t seed;
    };

    static_shaffle(static_shaffle const &) noexcept = default;
    static_shaffle(static_shaffle &&) noexcept      = default;
    static_shaffle &operator=(static_shaffle const &) noexcept =
        default;
    static_shaffle &operator=(static_shaffle &&) noexcept = default;

    explicit constexpr static_shaffle(_init init) noexcept;

    [[nodiscard]] constexpr auto operator[](uint64_t n) const noexcept
        -> uint64_t;

    [[nodiscard]] constexpr auto upper(uint64_t n) const noexcept
        -> uint64_t;

  private:
    struct _pair {
      uint64_t x, z;
    };

    [[nodiscard]] static constexpr auto splitmix64(
        uint64_t x) noexcept -> _pair;
    [[nodiscard]] static constexpr auto rotl(uint64_t x,
                                             int      k) noexcept
        -> uint64_t;

    constexpr auto gen() noexcept -> uint64_t;

    uint64_t m_begin    = 0;
    uint64_t m_end      = 65536;
    uint64_t m_seed     = 0;
    uint64_t m_state[2] = { 1, 2 };
  };
}

#include "static_shaffle.impl.h"

#endif
