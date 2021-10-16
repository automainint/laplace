/*  laplace/engine/eval/maze.h
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

#ifndef laplace_engine_eval_maze_h
#define laplace_engine_eval_maze_h

#include "../defs.h"

namespace laplace::engine::eval::maze {
  static constexpr int8_t walkable = 0;
  static constexpr int8_t wall     = 1;

  using fn_random = std::function<uint64_t()>;

  void generate(vec2z const             size,
                std::span<int8_t> const map,
                fn_random const         random);

  void stretch(vec2z const                   dst_size,
               std::span<int8_t> const       dst,
               vec2z const                   src_size,
               std::span<const int8_t> const src,
               sl::index const               tunnel_size,
               sl::index const               gate_size) noexcept;
}

#endif
