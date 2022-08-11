#ifndef LAPLACE_IO_IMPL_H
#define LAPLACE_IO_IMPL_H

#include "x_buffer.h"
#include "io_interface.h"
#include "options.h"
#include <random>

namespace laplace {
  struct io_impl : io_interface {
    std::mt19937_64 random;
    buffer          integers;
    byte_buffer     bytes;

    [[nodiscard]] auto clone() const noexcept
        -> std::shared_ptr<io_interface> override;

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type override;

    [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                byte_type def) const noexcept
        -> byte_type override;

    [[nodiscard]] auto apply(impact const &i) noexcept
        -> bool override;

    [[nodiscard]] auto adjust() noexcept -> bool override;

    void adjust_done() noexcept override;
  };
}

#endif
