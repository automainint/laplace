/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "length.h"

#include "../eval/integral.h"

namespace laplace::engine::tridi {
  auto length(vec3i const &v) noexcept -> intval {
    auto const _sqrt = [](intval x) { return eval::sqrt(x, 1); };
    return math::length(v, _sqrt);
  }
}
