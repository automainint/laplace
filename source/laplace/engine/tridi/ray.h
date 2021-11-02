/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_ray_h
#define laplace_engine_tridi_ray_h

#include "../defs.h"

namespace laplace::engine::tridi {
  struct ray {
    vec3i base;
    vec3i direction;

    [[nodiscard]] auto point(intval t) const noexcept -> vec3i;
    [[nodiscard]] auto square_distance(vec3i const &a) const noexcept
        -> intval;
    [[nodiscard]] auto square_distance(ray const &a) const noexcept
        -> intval;
  };
}

#endif
