/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_eval_triangle_h
#define laplace_engine_eval_triangle_h

#include "../defs.h"

#include "box.h"
#include "plane.h"

namespace laplace::engine::tridi {
  struct triangle {
    std::array<vec3i, 3> vertices;

    [[nodiscard]] auto get_center() const noexcept -> vec3i;
    [[nodiscard]] auto get_mass_center() const noexcept -> vec3i;
    [[nodiscard]] auto get_plane() const noexcept -> plane;
    [[nodiscard]] auto get_flat_center(sl::index axis) const noexcept
        -> intval;
    [[nodiscard]] auto get_bounds() const noexcept -> box;
    [[nodiscard]] auto plane_raw_normal() const noexcept -> vec3i;
    [[nodiscard]] auto orientation(vec3i const &point) const noexcept
        -> intval;
    [[nodiscard]] auto plane_square_distance(
        vec3i const &point) const noexcept -> intval;
    [[nodiscard]] auto plane_abs_square_distance(
        vec3i const &point) const noexcept -> intval;
    [[nodiscard]] auto contains_flat(
        vec3i const &point) const noexcept -> bool;
  };

  [[nodiscard]] auto bounds_of(std::span<triangle const> v) -> box;
}

#endif
