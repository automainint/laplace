#ifndef LAPLACE_X_IO_IMPL_TEST_H
#define LAPLACE_X_IO_IMPL_TEST_H

#include "../../laplace/x_io_interface.h"

namespace laplace::test {
  struct test_io_impl : io_interface {
    [[nodiscard]] auto clone() const noexcept
        -> std::shared_ptr<io_interface> override {
      return std::make_shared<test_io_impl>(*this);
    }

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type override {
      return 42;
    }

    [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                byte_type def) const noexcept
        -> byte_type override {
      return 24;
    }

    [[nodiscard]] auto apply(impact const &i) noexcept
        -> bool override {
      return true;
    }

    [[nodiscard]] auto adjust() noexcept -> bool override {
      return false;
    }

    void adjust_done() noexcept override { }
  };
}

#endif
