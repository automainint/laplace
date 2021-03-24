#pragma once

#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/quaternion.h"

namespace laplace::graphics
{
    using real = float;

    using vec2 = math::vector<2, real>;
    using vec3 = math::vector<3, real>;
    using vec4 = math::vector<4, real>;
    using mat3 = math::matrix<3, 3, real>;
    using mat4 = math::matrix<4, 4, real>;

    using ref_vec2 = vec2 &;
    using ref_vec3 = vec3 &;
    using ref_vec4 = vec4 &;
    using ref_mat3 = mat3 &;
    using ref_mat4 = mat4 &;

    using cref_vec2 = const vec2 &;
    using cref_vec3 = const vec3 &;
    using cref_vec4 = const vec4 &;
    using cref_mat3 = const mat3 &;
    using cref_mat4 = const mat4 &;
}
