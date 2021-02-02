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

#ifndef __laplace__engine_defs__
#define __laplace__engine_defs__

#include "../math/vector.h"
#include <array>
#include <variant>
#include <vector>

namespace laplace::engine {
  using vecval = int64_t;

  constexpr vecval epsilon    = 1;
  constexpr vecval safe_delta = 2;
  constexpr vecval infinity =
      std::numeric_limits<vecval>::max() >> 8;
  constexpr vecval safe_limit =
      std::numeric_limits<vecval>::max() >> 4;

  constexpr uint64_t time_undefined = -1;
  constexpr size_t   id_undefined   = -1;

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

#endif
