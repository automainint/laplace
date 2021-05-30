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

  void generate(const vec2z size, const std::span<int8_t> map,
                const fn_random random);

  void stretch(const vec2z dst_size, const std::span<int8_t> dst,
               const vec2z src_size, const std::span<const int8_t> src,
               const sl::index tunnel_size,
               const sl::index gate_size) noexcept;
}

#endif
