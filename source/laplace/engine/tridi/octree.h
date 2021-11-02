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
    box bounds;

    std::variant<sl::vector<octree>, sl::vector<triangle>> childs;

    [[nodiscard]] auto is_empty() const noexcept -> bool;
    [[nodiscard]] auto contains(vec3i const &point) const noexcept
        -> bool;
  };
}

#endif
