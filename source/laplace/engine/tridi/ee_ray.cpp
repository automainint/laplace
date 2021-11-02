/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "ray.h"

#include "../eval/integral.h"

namespace laplace::engine::tridi::impl {
  constexpr auto div3(auto const a, auto const s) {
    return decltype(a) { eval::div(a.x(), s, 1),
                         eval::div(a.y(), s, 1),
                         eval::div(a.z(), s, 1) };
  }
}

namespace laplace::engine::tridi {
  using impl::div3, std::abs;

  auto ray::point(intval t) const noexcept -> vec3i {

    const auto len = length(direction);

    if (len == 0) {
      return base;
    }

    return base + div3(direction * t, len);
  }

  auto ray::square_distance(vec3i const &a) const noexcept -> intval {
    auto r = a - base;

    if (abs(r.x()) + abs(r.y()) + abs(r.z()) <= epsilon)
      return 0;

    auto u = math::cross(direction, r);
    auto b = ray { a, u };

    return square_distance(b);
  }

  auto ray::square_distance(ray const &a) const noexcept -> intval {
    auto const l0 = length(direction);
    auto const l1 = length(a.direction);

    if (l0 == 0 || l1 == 0)
      return -safe_limit;

    auto const c = math::cross(direction, a.direction);
    auto const n = div3(c, l0 * l1);
    auto const r = a.base - base;
    auto const l = n * math::dot(r, n);

    return math::square_length(l);
  }
}
