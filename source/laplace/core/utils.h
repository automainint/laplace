/*  laplace/core/utils.h
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
   */
  [[nodiscard]] constexpr auto as_index(
      const uint64_t value, const sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert 64-bit integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      const int64_t value, const sl::index invalid = -1) noexcept
      -> sl::index;
}

#include "utils.impl.h"

#endif
