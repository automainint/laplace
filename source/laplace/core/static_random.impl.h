/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_STATIC_RANDOM_IMPL_H
#define LAPLACE_CORE_STATIC_RANDOM_IMPL_H

namespace laplace::core {
  constexpr static_random::static_random(uint64_t seed) noexcept {
    auto [x, z] = splitmix64(seed);
    m_state[0]  = z;
    m_state[1]  = splitmix64(x).z;
  }

  constexpr auto static_random::operator()() noexcept -> uint64_t {
    auto const s0 = m_state[0];
    auto       s1 = m_state[1];
    auto const z  = rotl(s0 + s1, 17) + s0;

    s1 ^= s0;
    m_state[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21);
    m_state[1] = rotl(s1, 28);

    return z;
  }

  constexpr auto static_random::splitmix64(uint64_t x) noexcept
      -> _pair {
    auto z = (x += 0x9E3779B97F4A7C15ull);
    z      = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z      = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return { x, z ^ (z >> 31) };
  }

  constexpr auto static_random::rotl(uint64_t x, int k) noexcept
      -> uint64_t {
    return (x << k) | (x >> (64 - k));
  }
}

#endif
