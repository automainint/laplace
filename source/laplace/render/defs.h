#pragma once

#include "../graphics/defs.h"
#include "../math/traits.h"

namespace laplace::render
{
    using timeval = int64_t;

    using graphics::real;
    using math::realmax_t;

    using graphics::vec2;
    using graphics::vec3;
    using graphics::vec4;
    using graphics::mat3;
    using graphics::mat4;

    using graphics::cref_vec2;
    using graphics::cref_vec3;
    using graphics::cref_vec4;
    using graphics::cref_mat3;
    using graphics::cref_mat4;

    /*  We need highest precision for quaternions.
     */
    using quat = math::quaternion<realmax_t>;
    using ref_quat = quat &;
    using cref_quat = const quat &;
}
