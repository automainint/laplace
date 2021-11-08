/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "sphere.h"

#include "../eval/integral.h"

namespace laplace::engine::tridi {
  using std::abs, eval::sqr, eval::add, eval::sub;

  auto sphere::is_empty() const noexcept -> bool {
    return radius < 0;
  }

  auto sphere::get_bounds() const noexcept -> box {
    return {
      .min = { sub(center.x(), radius), sub(center.y(), radius),
               sub(center.z(), radius) },
      .max = { add(center.x(), radius), add(center.y(), radius),
               add(center.z(), radius) }
    };
  }

  auto sphere::contains(vec3i const &point) const noexcept -> bool {
    if (auto dx = abs(sub(point.x(), center.x())); dx <= radius)
      if (auto dy = abs(sub(point.y(), center.y())); dy <= radius)
        if (auto dz = abs(sub(point.z(), center.z())); dz <= radius)
          return add(sqr(dx), add(sqr(dy), sqr(dz))) <= sqr(radius);
    return false;
  }
}
