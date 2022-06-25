/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "options.h"
#include <cstddef>
#include <variant>

namespace laplace {
  struct noop { };

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
    ptrdiff_t  id;
    ptrdiff_t  index;
    value_type value;
  };

  struct integer_add {
    ptrdiff_t  id;
    ptrdiff_t  index;
    value_type delta;
  };

  using impact =
      std::variant<noop, integer_reallocate, integer_allocate,
                   integer_deallocate, integer_set, integer_add>;
}

#endif
