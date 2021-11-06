/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_defs_h
#define laplace_engine_defs_h

#include "../core/defs.h"
#include "../math/vector.h"

namespace laplace::engine {
  using intval = int64_t;

  constexpr intval epsilon    = 1;
  constexpr intval safe_delta = 2;
  constexpr intval intval_max = std::numeric_limits<intval>::max();
  constexpr intval infinity   = intval_max / 16;
  constexpr intval safe_limit = intval_max / 4;

  constexpr sl::time  time_undefined = -1;
  constexpr sl::index id_undefined   = -1;

  using vec2i = math::vector<2, intval>;
  using vec3i = math::vector<3, intval>;
  using vec4i = math::vector<4, intval>;

  using vec2z = math::vector<2, sl::index>;
  using vec3z = math::vector<3, sl::index>;
  using vec4z = math::vector<4, sl::index>;
}

#endif
