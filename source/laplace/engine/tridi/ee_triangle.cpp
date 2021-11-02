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
#include <numeric>

namespace laplace::engine::tridi {
  using std::min, std::max, std::midpoint;

  auto triangle::get_center() const noexcept -> vec3i {
    return { get_flat_center(0), get_flat_center(1),
             get_flat_center(2) };
  }

  auto triangle::get_plane() const noexcept -> plane {
    return plane { vertices[0], plane_raw_normal() };
  }

  auto triangle::get_flat_center(sl::index axis) const noexcept
      -> intval {
    if (axis >= 0 && axis < 3) {
      auto p0 = min(min(vertices[0][axis], vertices[1][axis]),
                    vertices[2][axis]);
      auto p1 = max(max(vertices[0][axis], vertices[1][axis]),
                    vertices[2][axis]);
      return midpoint(p0, p1);
    }

    return {};
  }

  auto triangle::get_bounds() const noexcept -> box {
    return box {}.append(*this);
  }

  auto triangle::plane_raw_normal() const noexcept -> vec3i {
    auto a = vertices[1] - vertices[0];
    auto b = vertices[0] - vertices[2];
    return math::cross(a, b);
  }

  auto triangle::orientation(vec3i const &point) const noexcept
      -> intval {
    const auto n = plane_raw_normal();
    const auto r = point - vertices[0];
    return math::dot(n, r);
  }

  auto triangle::plane_square_distance(
      vec3i const &point) const noexcept -> intval {
    const auto n = plane_raw_normal();
    const auto l = math::square_length(n);

    if (l == 0)
      return safe_limit;

    const auto r = point - vertices[0];
    const auto d = math::dot(n, r);
    const auto x = eval::div(d * d, l, 1);
    return d < 0 ? -x : x;
  }

  auto triangle::plane_abs_square_distance(
      vec3i const &point) const noexcept -> intval {
    const auto n = plane_raw_normal();
    const auto l = math::square_length(n);

    if (l != 0) {
      const auto r = point - vertices[0];
      const auto d = math::dot(n, r);
      return eval::div(d * d, l, 1);
    }

    return safe_limit;
  }

  auto triangle::contains_flat(vec3i const &point) const noexcept
      -> bool {
    for (sl::index i = 0; i < 3; i++) {
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

  auto bounds_of(std::span<triangle const> v) -> box {
    auto bounds = box {};
    for (auto const &tri : v) bounds = bounds.append(tri);
    return bounds;
  }
}
