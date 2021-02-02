#pragma once

#include <cstdint>

namespace laplace::engine::object::sets
{
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

    enum set_id : size_t
    {
        undefined = 0,

        is_dynamic,
        is_markable,
        is_selectable,
        is_vulnerable,
        tick_period,
        bounds_min_x,
        bounds_min_y,
        bounds_min_z,
        bounds_max_x,
        bounds_max_y,
        bounds_max_z,

        debug_value,
        is_local,

        /*  Total native set count.
         */
        _native_count
    };
}
