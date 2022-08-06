#ifndef LAPLACE_IO_INTERFACE_H
#define LAPLACE_IO_INTERFACE_H

#include "options.h"
#include <memory>

namespace laplace {
  class impact;

  struct io_interface {
    virtual ~io_interface() noexcept = default;

    [[nodiscard]] virtual auto clone() const noexcept
        -> std::shared_ptr<io_interface> = 0;

    [[nodiscard]] virtual auto get_integer(
        ptrdiff_t id, ptrdiff_t index, int_type def) const noexcept
        -> int_type = 0;

    [[nodiscard]] virtual auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                        byte_type def) const noexcept
        -> byte_type = 0;

    [[nodiscard]] virtual auto apply(impact const &i) noexcept
        -> bool = 0;

    [[nodiscard]] virtual auto adjust() noexcept -> bool = 0;

    virtual void adjust_done() noexcept = 0;
  };
}

#endif
