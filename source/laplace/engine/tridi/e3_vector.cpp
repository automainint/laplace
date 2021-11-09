/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "vector.h"

#include "../eval/integral.h"

namespace laplace::engine::tridi {
  auto add(vec3i const &a, vec3i const &b) noexcept -> vec3i {
    return math::op(a, b, eval::op_add);
  }

  auto sub(vec3i const &a, vec3i const &b) noexcept -> vec3i {
    return math::op(a, b, eval::op_sub);
  }

  auto mul(vec3i const &v, intval x) noexcept -> vec3i {
    return math::op(v, x, eval::op_mul);
  }

  auto div(vec3i const &v, intval x) noexcept -> vec3i {
    return math::op(v, x, eval::op_div);
  }

  auto mul(intval x, vec3i const &v) noexcept -> vec3i {
    return math::op(x, v, eval::op_mul);
  }

  auto div(intval x, vec3i const &v) noexcept -> vec3i {
    return math::op(x, v, eval::op_div);
  }

  auto dot(vec3i const &a, vec3i const &b) noexcept -> intval {
    return math::dot(a, b, eval::op_add, eval::op_mul);
  }

  auto cross(vec3i const &a, vec3i const &b) noexcept -> vec3i {
    return math::cross(a, b, eval::op_sub, eval::op_mul);
  }

  auto square_length(vec3i const &v) noexcept -> intval {
    return math::square_length(v, eval::op_add, eval::op_mul);
  }

  auto length(vec3i const &v) noexcept -> intval {
    return eval::sqrt(square_length(v), 1);
  }
}
