/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_STATIC_SHAFFLE_IMPL_H
#define LAPLACE_CORE_STATIC_SHAFFLE_IMPL_H

namespace laplace::core {
  template <uint64_t begin_, uint64_t end_>
  constexpr static_shaffle<begin_, end_>::static_shaffle(
      uint64_t seed) noexcept {
    auto gen = static_random { seed };
    uint64_t buf[end_ - begin_] = {};
    for (uint64_t i = 0; begin_ + i < end_; i++)
      buf[i] = begin_ + i;
    m_values[0] = buf[0];
    for (uint64_t i = 1; begin_ + i < end_; i++) {
      auto m = gen() % (i + 1);
      for (auto j = i; j > m; j--)
        m_values[j] = m_values[j - 1];
      m_values[m] = buf[i];
    }
  }

  template <uint64_t begin_, uint64_t end_>
  constexpr auto static_shaffle<begin_, end_>::operator[](
      uint64_t n) const noexcept -> uint64_t {
    if (begin_ + n >= end_)
      return 0;
    return m_values[n];
  }

  template <uint64_t begin_, uint64_t end_>
  constexpr auto static_shaffle<begin_, end_>::upper(
      uint64_t n) const noexcept -> uint64_t {
    auto y = uint64_t{};
    for (uint64_t i = 0; i < n && begin_ + i < end_; i++)
      if (y <= m_values[i])
        y = m_values[i] + 1;
    return y;
  }

  static_assert(static_shaffle<0, 256>(1)[2] !=
                static_shaffle<0, 256>(1)[18]);
}

#endif
