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
#include "vector.h"

namespace laplace::engine::tridi {
  using eval::mul;

  auto ray::point(intval const t) const noexcept -> vec3i {
    auto const len = length(direction);

    if (len == 0)
      return base;

    return add(base, div(mul(direction, t), len));
  }

  auto ray::square_distance(vec3i const &a) const noexcept -> intval {
    auto const r = sub(a, base);

    if (r.x() == 0 && r.y() == 0 && r.z() == 0)
      return 0;

    auto const u = cross(direction, r);
    auto const b = ray { a, u };

    return square_distance(b);
  }

  auto ray::square_distance(ray const &a) const noexcept -> intval {
    /*  TODO
     *  Improve precision if possible.
     */

    auto const l0 = length(direction);
    auto const l1 = length(a.direction);

    if (l0 == 0 || l1 == 0)
      return -safe_limit;

    auto const c = cross(direction, a.direction);
    auto const n = div(c, mul(l0, l1));
    auto const r = sub(a.base, base);
    auto const l = mul(n, dot(r, n));

    return square_length(l);
  }

  auto segment::get_ray(sl::index n) const noexcept -> ray {
    if (n == 0)
      return ray { .base = begin, .direction = sub(end, begin) };
    return ray { .base = end, .direction = sub(begin, end) };
  }
}
