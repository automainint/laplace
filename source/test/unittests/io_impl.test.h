/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IO_IMPL_TEST_H
#define LAPLACE_IO_IMPL_TEST_H

#include "../../laplace/io_interface.h"

#include <iostream>

namespace laplace::test {
  struct test_io_impl : io_interface {
    test_io_impl() {
      std::cout << "custom_io_impl()\n";
    }

    [[nodiscard]] auto clone() const noexcept
        -> std::shared_ptr<io_interface> override {
      return std::make_shared<test_io_impl>(*this);
    }
    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type override {
      std::cout << "get_integer()\n";
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
