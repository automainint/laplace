/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_STATE_H
#define LAPLACE_STATE_H

#include "buffer.h"
#include "impact.h"
#include "options.h"
#include <random>

namespace laplace {
  class state {
  public:
    state() noexcept;

    [[nodiscard]] auto set_seed(int_type seed) const noexcept
        -> state;

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type;

    [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                byte_type def) const noexcept
        -> byte_type;

    [[nodiscard]] auto apply(impact const &i) noexcept -> bool;

    [[nodiscard]] auto adjust() noexcept -> bool;
    void               adjust_done() noexcept;

  private:
    std::mt19937_64       m_random;
    buffer                m_integers;
    byte_buffer           m_bytes;
  };
}

#endif
