/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_octree_h
#define laplace_engine_tridi_octree_h

#include "box.h"
#include "triangle.h"

namespace laplace::engine::tridi {
  struct octree {
    using children_type =
        std::variant<sl::vector<octree>, sl::vector<triangle>>;

    enum children_index : sl::index {
      n_octrees   = 0,
      n_triangles = 1
    };

    box           bounds;
    children_type children;

    [[nodiscard]] auto is_empty() const noexcept -> bool;
    [[nodiscard]] auto contains(vec3i const &point) const noexcept
        -> bool;
  };
}

#endif
