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

namespace laplace::engine::tridi {
  using std::abs;

  auto sphere::is_empty() const noexcept -> bool {
    return radius < 0;
  }

  auto sphere::get_bounds() const noexcept -> box {
    return { .min = { center.x() - radius, center.y() - radius,
                      center.z() - radius },
             .max = { center.x() + radius, center.y() + radius,
                      center.z() + radius } };
  }

  auto sphere::contains(vec3i const &point) const noexcept -> bool {
    if (auto dx = abs(point.x() - center.x()); dx - epsilon < radius)
      if (auto dy = abs(point.y() - center.y());
          dy - epsilon < radius)
        if (auto dz = abs(point.z() - center.z());
            dz - epsilon < radius)
          return dx * dx + dy * dy + dz * dz - epsilon <
                 radius * radius;
    return false;
  }
}
