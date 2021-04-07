/*  laplace/format/utils.h
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

#ifndef laplace_format_utils_h
#define laplace_format_utils_h

#include "defs.h"

namespace laplace::format {
  auto wrap(std::istream &in) -> fn_read;
  auto wrap(std::ostream &out) -> fn_write;
}

#include "utils.impl.h"

#endif
