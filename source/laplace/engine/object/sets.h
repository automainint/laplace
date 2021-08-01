/*  laplace/engine/object/sets.h
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

#ifndef laplace_engine_object_sets_h
#define laplace_engine_object_sets_h

#include "../defs.h"

namespace laplace::engine::object::sets {
  /*  For removed units.
   */
  static constexpr intval zombie_duration = 100000;

  /*  Health points, experience points, etc.
   */
  static constexpr intval scale_points = 100;

  /*  Any real value.
   */
  static constexpr intval scale_real = 1000;

  /*  Values in virtual seconds.
   */
  static constexpr intval scale_time = 1000;

  /*  Values of coefficients.
   */
  static constexpr intval scale_factor = 10000;

  /*  Values in 2 Pi units (360 degree).
   */
  static constexpr intval scale_angle = scale_factor;

  static_assert(scale_points > 0);
  static_assert(scale_real > 0);
  static_assert(scale_time > 0);
  static_assert(scale_factor > 0);
  static_assert(scale_angle > 0);

  enum set_id : sl::index {
    undefined = 0,

    is_dynamic,
    tick_period,

    debug_value,
    is_local,

    /*  Total native set count.
     */
    _native_count
  };
}

#endif
