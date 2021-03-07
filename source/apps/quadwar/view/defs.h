/*  apps/quadwar/view/defs.h
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

#ifndef quadwar_view_defs_h
#define quadwar_view_defs_h

#include "../../../laplace/engine/helper.h"
#include "../../../laplace/math/vector.h"
#include "../defs.h"

namespace quadwar_app::view {
  using real = float;

  using vec2 = math::vector<2, real>;
  using vec3 = math::vector<3, real>;
  using vec4 = math::vector<4, real>;
}

#endif
