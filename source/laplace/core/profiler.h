/*  laplace/core/profiler.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_profiler_h
#define laplace_core_profiler_h

#include "slib.h"
#include <functional>

namespace laplace::core::profiler {
  using fn_segment =
      std::function<void(std::string_view begin,
                         std::string_view end,
                         sl::time         duration_microseconds)>;

  void setup(fn_segment fn) noexcept;
  void fence(std::string_view name) noexcept;
}

#endif
