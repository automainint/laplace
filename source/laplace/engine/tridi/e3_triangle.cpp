/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "triangle.h"

#include "../eval/integral.h"
#include "vector.h"
#include <numeric>

namespace laplace::engine::tridi {
  using std::min, std::max, std::midpoint, eval::div_sum, eval::div,
      eval::sqr;

  auto triangle::get_center() const noexcept -> vec3i {
    return { get_flat_center(0), get_flat_center(1),
             get_flat_center(2) };
  }

  auto triangle::get_mass_center() const noexcept -> vec3i {
    return {
      div_sum(vertices[0].x(), vertices[1].x(), vertices[2].x(), 3),
      div_sum(vertices[0].y(), vertices[1].y(), vertices[2].y(), 3),
      div_sum(vertices[0].z(), vertices[1].z(), vertices[2].z(), 3)
    };
  }

  auto triangle::get_plane() const noexcept -> plane {
    return plane { vertices[0], plane_raw_normal() };
  }

  auto triangle::get_flat_center(sl::index axis) const noexcept
      -> intval {
    if (axis >= 0 && axis < 3) {
      auto const p0 = min(min(vertices[0][axis], vertices[1][axis]),
                          vertices[2][axis]);
      auto const p1 = max(max(vertices[0][axis], vertices[1][axis]),
                          vertices[2][axis]);
      return midpoint(p0, p1);
    }

    return {};
  }

  auto triangle::get_bounds() const noexcept -> box {
    return box {}.append(*this);
  }

  auto triangle::plane_raw_normal() const noexcept -> vec3i {
    auto const a = sub(vertices[1], vertices[0]);
    auto const b = sub(vertices[0], vertices[2]);
    return cross(a, b);
  }

  auto triangle::orientation(vec3i const &point) const noexcept
      -> intval {
    auto const n = plane_raw_normal();
    auto const r = sub(point, vertices[0]);
    return dot(n, r);
  }

  auto triangle::plane_square_distance(
      vec3i const &point) const noexcept -> intval {
    auto const n = plane_raw_normal();
    auto const l = square_length(n);

    if (l == 0)
      return safe_limit;

    auto const r = sub(point, vertices[0]);
    auto const d = dot(n, r);
    auto const x = div(sqr(d), l);
    return d < 0 ? -x : x;
  }

  auto triangle::plane_abs_square_distance(
      vec3i const &point) const noexcept -> intval {
    auto const n = plane_raw_normal();
    auto const l = square_length(n);

    if (l == 0)
      return safe_limit;

    auto const r = sub(point, vertices[0]);
    auto const d = dot(n, r);
    return div(sqr(d), l);
  }

  auto triangle::contains_flat(vec3i const &point) const noexcept
      -> bool {
    for (sl::index i = 0; i < 3; i++) {
      auto const i1 = (i == 2 ? 0 : i + 1);
      auto const i2 = (i == 0 ? 2 : i - 1);

      auto const v0 = sub(vertices[i1], vertices[i]);
      auto const v1 = sub(vertices[i2], vertices[i]);
      auto const p  = sub(point, vertices[i]);

      auto const r0 = cross(v0, v1);
      auto const r1 = cross(v0, p);

      if (dot(r0, r1) < 0)
        return false;
    }

    return true;
  }
}
