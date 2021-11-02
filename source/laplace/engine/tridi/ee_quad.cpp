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

namespace laplace::engine::tridi {
  auto quad::get_plane() const noexcept -> plane {
    return plane { vertices[0], raw_normal(vertices[0], vertices[1],
                                           vertices[2]) };
  }

  auto quad::bounds_of() const noexcept -> box {
    return box {}.append(*this);
  }

  auto quad::contains_flat(vec3i const &point) const noexcept
      -> bool {
    for (sl::index i = 0; i < 4; i++) {
      const auto i1 = (i == 2 ? 0 : i + 1);
      const auto i2 = (i == 0 ? 2 : i - 1);

      const auto v0 = vertices[i1] - vertices[i];
      const auto v1 = vertices[i2] - vertices[i];
      const auto p  = point - vertices[i];

      const auto r0 = math::cross(v0, v1);
      const auto r1 = math::cross(v0, p);

      if (math::dot(r0, r1) + epsilon <= 0)
        return false;
    }

    return true;
  }

}
