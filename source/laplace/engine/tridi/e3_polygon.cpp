/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "polygon.h"

#include "vector.h"

namespace laplace::engine::tridi {
  polygon::polygon(std::initializer_list<vec3i> v) noexcept :
      vertices(v) { }

  auto polygon::get_plane() const noexcept -> plane {
    if (vertices.size() < 3)
      return plane {};
    return plane { vertices[0], raw_normal(vertices[0], vertices[1],
                                           vertices[2]) };
  }
}
