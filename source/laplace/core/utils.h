/*  laplace/core/utils.h
 *
 *      The core utilities.
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

#ifndef laplace_core_utils_h
#define laplace_core_utils_h

#include "bytestream.h"
#include "defs.h"
#include "string.h"
#include "utf8.h"

namespace laplace {
  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   *
   *  Returns -1 for invalid index.
   */
  [[nodiscard]] constexpr auto as_index(uint64_t value) noexcept -> sl::index;

  [[nodiscard]] constexpr auto as_index(int64_t value) noexcept -> sl::index;
}

#include "utils.impl.h"

#endif
