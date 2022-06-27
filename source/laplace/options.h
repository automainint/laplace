/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#include <cstddef>
#include <cstdint>

namespace laplace {
  using time_type = signed long long;
  using int_type  = signed long long;
  using byte_type = int8_t;

  static constexpr ptrdiff_t id_undefined    = -1;
  static constexpr ptrdiff_t index_undefined = -1;

  extern ptrdiff_t const average_impact_list_size;
  extern ptrdiff_t const average_impact_count;
}

#endif
