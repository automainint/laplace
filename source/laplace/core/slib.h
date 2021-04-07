/*  laplace/core/slib.h
 *
 *      The Laplace support library.
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

#ifndef laplace_core_slib_h
#define laplace_core_slib_h

#include "log.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace laplace::sl {
  /*  TODO
   *  Small vector optimization.
   */
  template <typename type_, typename alloc_ = std::allocator<type_>>
  using vector = std::vector<type_, alloc_>;

  /*  For indexing integer values.
   */
  using index = ptrdiff_t;

  /*  Integer value that shall not be negative.
   */
  using whole = ptrdiff_t;
}

#endif
