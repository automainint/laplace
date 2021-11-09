/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_cylinder_h
#define laplace_engine_tridi_cylinder_h

#include "box.h"
#include "plane.h"
#include "ray.h"

namespace laplace::engine::tridi {
  struct cylinder {
    vec3i  base   = { 0, 0, 0 };
    intval radius = -1;
    intval height = -1;

    [[nodiscard]] auto is_empty() const noexcept -> bool;
    [[nodiscard]] auto get_bounds() const noexcept -> box;
    [[nodiscard]] auto get_floor_z() const noexcept -> intval;
    [[nodiscard]] auto get_roof_z() const noexcept -> intval;
    [[nodiscard]] auto contains_flat(
        vec3i const &point) const noexcept -> bool;
    [[nodiscard]] auto contains(vec3i const &point) const noexcept
        -> bool;
  };
}

#endif
