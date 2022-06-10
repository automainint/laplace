/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#include <cstddef>

namespace laplace {
  using time_type  = signed long long;
  using value_type = signed long long;

  static constexpr ptrdiff_t id_undefined    = -1;
  static constexpr ptrdiff_t index_undefined = -1;
}

#endif
