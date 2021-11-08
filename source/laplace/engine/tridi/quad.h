/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_quad_h
#define laplace_engine_tridi_quad_h

#include "box.h"
#include "plane.h"

namespace laplace::engine::tridi {
  struct quad {
    std::array<vec3i, 4> vertices;

    [[nodiscard]] auto get_plane() const noexcept -> plane;
    [[nodiscard]] auto get_bounds() const noexcept -> box;
    [[nodiscard]] auto contains_flat(
        vec3i const &point) const noexcept -> bool;
  };
}

#endif
