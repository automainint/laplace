/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ACCESS_H
#define LAPLACE_ACCESS_H

#include "state.h"
#include <optional>

namespace laplace {
  /*  Read-only access to the state.
   */
  class access {
  public:
    access() noexcept = default;
    explicit access(state const &s) noexcept;

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type;

    [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                byte_type def) const noexcept
        -> byte_type;

    [[nodiscard]] auto operator==(access const &) const noexcept
        -> bool = default;

  private:
    std::optional<std::shared_ptr<io_interface>> m_io;
  };
}

#include "access.impl.h"

#endif
