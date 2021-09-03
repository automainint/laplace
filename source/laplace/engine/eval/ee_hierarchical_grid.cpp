/*  laplace/engine/eval/ee_hierarchical_grid.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "hierarchical_grid.h"

namespace laplace::engine::eval::hierarchical_grid {
  using std::span;

  auto gen(const vec2z        size,
           const sl::whole    scale,
           span<const int8_t> map,
           fn_available       available,
           const vec2z block_size) noexcept -> sl::vector<node> {
    return {};
  }
}
