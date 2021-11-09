/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "plane.h"

#include "vector.h"

namespace laplace::engine::tridi {
  auto raw_normal(vec3i const &p0,
                  vec3i const &p1,
                  vec3i const &p2) noexcept -> vec3i {
    auto const a = sub(p1, p0);
    auto const b = sub(p0, p2);
    return cross(a, b);
  }

  auto operator==(plane const &a, plane const &b) noexcept -> bool {
    return a.base == b.base && a.normal == b.normal;
  }

  auto operator!=(plane const &a, plane const &b) noexcept -> bool {
    return a.base != b.base || a.normal != b.normal;
  }
}
