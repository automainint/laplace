/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "options.h"
#include <cstddef>
#include <variant>

namespace laplace {
  struct noop { };
  struct tick_continue { };

  struct integer_reserve {
    ptrdiff_t count;
  };

  struct integer_reallocate {
    ptrdiff_t id;
    ptrdiff_t size;
  };

  struct integer_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;
  };

  struct integer_deallocate {
    ptrdiff_t id;
  };

  struct integer_set {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  value;
  };

  struct integer_add {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  delta;
  };

  struct byte_reserve {
    ptrdiff_t count;
  };

  struct byte_reallocate {
    ptrdiff_t id;
    ptrdiff_t size;
  };

  struct byte_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;
  };

  struct byte_deallocate {
    ptrdiff_t id;
  };

  struct byte_set {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type value;
  };

  struct byte_add {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type delta;
  };

  struct random {
    int_type  min;
    int_type  max;
    ptrdiff_t return_id;
    ptrdiff_t return_index;
    ptrdiff_t return_size;
  };

  using impact = std::variant<
      noop, tick_continue, integer_reserve, integer_reallocate,
      integer_allocate, integer_deallocate, integer_set, integer_add,
      byte_reserve, byte_reallocate, byte_allocate, byte_deallocate,
      byte_set, byte_add, random>;
}

#endif
