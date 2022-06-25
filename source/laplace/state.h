/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_STATE_H
#define LAPLACE_STATE_H

#include "buffer.h"
#include "impact.h"
#include "options.h"

namespace laplace {
  class state {
  public:
    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto apply(impact const &i) noexcept -> bool;

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   value_type def) const noexcept
        -> value_type;

    [[nodiscard]] auto adjust() noexcept -> bool;
    void adjust_done() noexcept;

  private:
    buffer m_integers;
  };
}

#endif
