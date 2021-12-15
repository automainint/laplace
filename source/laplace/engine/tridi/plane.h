/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_plane_h
#define laplace_engine_tridi_plane_h

#include "../defs.h"

namespace laplace::engine::tridi {
  struct plane {
    vec3i base;
    vec3i normal;
  };

  auto raw_normal(vec3i const &p0,
                  vec3i const &p1,
                  vec3i const &p2) noexcept -> vec3i;

  [[nodiscard]] auto operator==(plane const &a,
                                plane const &b) noexcept -> bool;

  [[nodiscard]] auto operator!=(plane const &a,
                                plane const &b) noexcept -> bool;
}

#endif
