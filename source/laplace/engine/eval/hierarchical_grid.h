/*  laplace/engine/eval/hierarchical_grid.h
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

#ifndef laplace_engine_eval_hierarchical_grid_h
#define laplace_engine_eval_hierarchical_grid_h

#include "grid.h"

namespace laplace::engine::eval::hierarchical_grid {
  using astar::link;
  using grid::fn_available;

  struct node {
    vec2z            block;
    vec2z            position;
    sl::vector<link> neighbors;
  };

  [[nodiscard]] auto gen(const vec2z             size,
                         const sl::whole         scale,
                         std::span<const int8_t> map,
                         fn_available            available,
                         const vec2z             block_size) noexcept
      -> sl::vector<node>;
}

#endif
