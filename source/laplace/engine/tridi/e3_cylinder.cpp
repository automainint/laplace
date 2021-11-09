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

#include "../eval/integral.h"

namespace laplace::engine::tridi {
  using std::abs, eval::add, eval::sub, eval::sqr;

  auto cylinder::is_empty() const noexcept -> bool {
    return radius < 0 || height < 0;
  }

  auto cylinder::get_bounds() const noexcept -> box {
    return { .min = { sub(base.x(), radius), sub(base.y(), radius),
                      base.z() },
             .max = { add(base.x(), radius), add(base.y(), radius),
                      add(base.z(), height) } };
  }

  auto cylinder::get_floor_z() const noexcept -> intval {
    return base.z();
  }

  auto cylinder::get_roof_z() const noexcept -> intval {
    return add(base.z(), height);
  }

  auto cylinder::contains_flat(vec3i const &point) const noexcept
      -> bool {
    if (auto dx = abs(sub(point.x(), base.x())); dx <= radius)
      if (auto dy = abs(sub(point.y(), base.y())); dy <= radius)
        return add(sqr(dx), sqr(dy)) <= sqr(radius);
    return false;
  }

  auto cylinder::contains(vec3i const &point) const noexcept -> bool {
    return base.z() <= point.z() &&
           add(base.z(), height) >= point.z() && contains_flat(point);
  }
}
