/*  laplace/engine/defs.h
 *
 *      Basic engine definitions. All the gameplay values are
 *      fixed-point numbers.
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

#pragma once

#include "../math/vector.h"
#include <variant>
#include <vector>
#include <array>

namespace laplace::engine
{
    using vecval = int64_t;

    static constexpr vecval epsilon     = 1;
    static constexpr vecval safe_delta  = 2;
    static constexpr vecval infinity    = std::numeric_limits<vecval>::max() >> 8;
    static constexpr vecval safe_limit  = std::numeric_limits<vecval>::max() >> 4;

    using vec2i = math::vector<2, vecval>;
    using vec3i = math::vector<3, vecval>;
    using vec4i = math::vector<4, vecval>;

    using ref_vec2i = vec2i &;
    using ref_vec3i = vec3i &;
    using ref_vec4i = vec4i &;

    using cref_vec2i = const vec2i &;
    using cref_vec3i = const vec3i &;
    using cref_vec4i = const vec4i &;
}
