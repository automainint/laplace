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

#include <cstdint>

namespace laplace::engine::object::sets {
  /*  For removed units.
   */
  static constexpr int64_t zombie_duration = 100000;

  /*  Health points, experience points, etc.
   */
  static constexpr int64_t scale_points = 100;

  /*  Any real value.
   */
  static constexpr int64_t scale_real = 1000;

  /*  Values in virtual seconds.
   */
  static constexpr int64_t scale_time = 1000;

  /*  Values of coefficients.
   */
  static constexpr int64_t scale_factor = 10000;

  /*  Values in 2 Pi units (360 degree).
   */
  static constexpr int64_t scale_angle = scale_factor;

  enum set_id : size_t {
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
