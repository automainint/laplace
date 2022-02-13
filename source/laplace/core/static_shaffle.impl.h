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
  constexpr static_shaffle::static_shaffle(_init init) noexcept :
      m_begin(init.begin), m_end(std::max(init.begin + 1, init.end)),
      m_seed(init.seed) {
    auto [x, z] = splitmix64(m_seed);
    m_state[0]  = z;
    m_state[1]  = splitmix64(x).z;
  }

  constexpr auto static_shaffle::operator[](uint64_t n) const noexcept
      -> uint64_t {
    if (n >= m_end - m_begin)
      return 0;
    auto s = static_shaffle { *this };
    auto z = uint64_t {};
    for (auto m = 0;; m++) {
      z = s.gen();
      if ([](static_shaffle s, uint64_t m, uint64_t z) {
            for (; m > 0; m--)
              if (s.gen() == z)
                return true;
            return false;
          }(*this, m, z))
        continue;
      if (n == 0)
        break;
      n--;
    }
    return z;
  }

  constexpr auto static_shaffle::upper(uint64_t n) const noexcept
      -> uint64_t {
    if (n >= m_end - m_begin)
      return m_end - m_begin;
    auto y = 0;
    auto s = static_shaffle { *this };
    for (auto m = 0; n > 0; m++) {
      auto z = s.gen();
      if (y <= z)
        y = z + 1;
      if ([s = *this](uint64_t m, uint64_t z) mutable {
            for (uint64_t k = 0; k < m; k++)
              if (s.gen() == z)
                return false;
            return true;
          }(m, z))
        n--;
    }
    return y;
  }

  constexpr auto static_shaffle::splitmix64(uint64_t x) noexcept
      -> _pair {
    auto z = (x += 0x9E3779B97F4A7C15ull);
    z      = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z      = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return { x, z ^ (z >> 31) };
  }

  constexpr auto static_shaffle::rotl(uint64_t x, int k) noexcept
      -> uint64_t {
    return (x << k) | (x >> (64 - k));
  }

  constexpr auto static_shaffle::gen() noexcept -> uint64_t {
    auto const s0 = m_state[0];
    auto       s1 = m_state[1];
    auto const z  = rotl(s0 + s1, 17) + s0;

    s1 ^= s0;
    m_state[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21);
    m_state[1] = rotl(s1, 28);

    return m_begin + (z % (m_end - m_begin));
  }

  static_assert(static_shaffle({ 0, 256, 1 })[2] !=
                static_shaffle({ 0, 256, 1 })[18]);
}

#endif
