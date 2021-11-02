/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_sphere_h
#define laplace_engine_tridi_sphere_h

#include "box.h"

namespace laplace::engine::tridi {
  struct sphere {
    vec3i  center = { 0, 0, 0 };
    intval radius = -safe_delta;

    [[nodiscard]] auto is_empty() const noexcept -> bool;
    [[nodiscard]] auto get_bounds() const noexcept -> box;
    [[nodiscard]] auto contains(vec3i const &point) const noexcept
        -> bool;
  };
}

#endif
