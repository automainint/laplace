/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "quad.h"

#include "vector.h"

namespace laplace::engine::tridi {
  auto quad::get_plane() const noexcept -> plane {
    return plane { vertices[0], raw_normal(vertices[0], vertices[1],
                                           vertices[2]) };
  }

  auto quad::get_bounds() const noexcept -> box {
    return box {}.append(*this);
  }

  auto quad::contains_flat(vec3i const &point) const noexcept
      -> bool {
    for (sl::index i = 0; i < 4; i++) {
      const auto i1 = (i == 3 ? 0 : i + 1);
      const auto i2 = (i == 0 ? 3 : i - 1);

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
