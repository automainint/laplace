/*  laplace/engine/object/eo_sets.cpp
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

#include "sets.h"

namespace laplace::engine::object::sets {
  static_assert(scale_points > 0);
  static_assert(scale_real > 0);
  static_assert(scale_time > 0);
  static_assert(scale_factor > 0);
  static_assert(scale_angle > 0);
}
