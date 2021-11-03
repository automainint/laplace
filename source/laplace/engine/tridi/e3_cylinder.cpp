/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "cylinder.h"

namespace laplace::engine::tridi {
  using std::abs;

  auto cylinder::is_empty() const noexcept -> bool {
    return radius < 0 || height < 0;
  }

  auto cylinder::get_bounds() const noexcept -> box {
    return { .min = { base.x() - radius, base.y() - radius,
                      base.z() },
             .max = { base.x() + radius, base.y() + radius,
                      base.z() + height } };
  }

  auto cylinder::contains_flat(vec3i const &point) const noexcept
      -> bool {
    if (auto dx = abs(point.x() - base.x()); dx - epsilon < radius)
      if (auto dy = abs(point.y() - base.y()); dy - epsilon < radius)
        return dx * dx + dy * dy - epsilon < radius * radius;
    return false;
  }

  auto cylinder::contains(vec3i const &point) const noexcept -> bool {
    return base.z() - epsilon < point.z() &&
           base.z() + height + epsilon > point.z() &&
           contains_flat(point);
  }
}
