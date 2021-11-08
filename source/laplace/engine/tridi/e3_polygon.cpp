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

  auto polygon::get_bounds() const noexcept -> box {
    return box {}.append(*this);
  }

  auto polygon::contains_flat(vec3i const &point) const noexcept
      -> bool {
    for (sl::index i = 0; i < vertices.size(); i++) {
      const auto i1 = (i == vertices.size() - 1 ? 0 : i + 1);
      const auto i2 = (i == 0 ? vertices.size() - 1 : i - 1);

      const auto v0 = sub(vertices[i1], vertices[i]);
      const auto v1 = sub(vertices[i2], vertices[i]);
      const auto p  = sub(point, vertices[i]);

      const auto r0 = cross(v0, v1);
      const auto r1 = cross(v0, p);

      if (dot(r0, r1) < 0)
        return false;
    }

    return true;
  }
}
