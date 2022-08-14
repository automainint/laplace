#ifndef LAPLACE_X_OPERATIONS_H
#define LAPLACE_X_OPERATIONS_H

#include "options.h"
#include <cstddef>

namespace laplace {
  struct noop {
    [[nodiscard]] auto operator==(noop const &) const noexcept
        -> bool = default;
  };

  struct tick_continue {
    [[nodiscard]] auto operator==(
        tick_continue const &) const noexcept -> bool = default;
  };

  struct integer_reserve {
    ptrdiff_t count;

    [[nodiscard]] auto operator==(
        integer_reserve const &) const noexcept -> bool = default;
  };

  struct integer_allocate_into {
    ptrdiff_t id;
    ptrdiff_t size;

    [[nodiscard]] auto operator==(integer_allocate_into const &)
        const noexcept -> bool = default;
  };

  struct integer_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;

    [[nodiscard]] auto operator==(
        integer_allocate const &) const noexcept -> bool = default;
  };

  struct integer_deallocate {
    ptrdiff_t id;

    [[nodiscard]] auto operator==(
        integer_deallocate const &) const noexcept -> bool = default;
  };

  struct integer_set {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  value;

    [[nodiscard]] auto operator==(integer_set const &) const noexcept
        -> bool = default;
  };

  struct integer_add {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  delta;

    [[nodiscard]] auto operator==(integer_add const &) const noexcept
        -> bool = default;
  };

  struct byte_reserve {
    ptrdiff_t count;

    [[nodiscard]] auto operator==(byte_reserve const &) const noexcept
        -> bool = default;
  };

  struct byte_allocate_into {
    ptrdiff_t id;
    ptrdiff_t size;

    [[nodiscard]] auto operator==(
        byte_allocate_into const &) const noexcept -> bool = default;
  };

  struct byte_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;

    [[nodiscard]] auto operator==(
        byte_allocate const &) const noexcept -> bool = default;
  };

  struct byte_deallocate {
    ptrdiff_t id;

    [[nodiscard]] auto operator==(
        byte_deallocate const &) const noexcept -> bool = default;
  };

  struct byte_set {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type value;

    [[nodiscard]] auto operator==(byte_set const &) const noexcept
        -> bool = default;
  };

  struct byte_add {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type delta;

    [[nodiscard]] auto operator==(byte_add const &) const noexcept
        -> bool = default;
  };

  struct integer_seed {
    int_type seed;

    [[nodiscard]] auto operator==(integer_seed const &) const noexcept
        -> bool = default;
  };

  struct integer_random {
    int_type  min;
    int_type  max;
    ptrdiff_t return_id;
    ptrdiff_t return_index;
    ptrdiff_t return_size;

    [[nodiscard]] auto operator==(
        integer_random const &) const noexcept -> bool = default;
  };
}

#endif
