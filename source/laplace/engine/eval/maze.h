/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_EVAL_MAZE_H
#define LAPLACE_ENGINE_EVAL_MAZE_H

#include "../defs.h"

namespace laplace::engine::eval::maze {
  static constexpr int8_t walkable = 0;
  static constexpr int8_t wall     = 1;

  using fn_random = std::function<uint64_t()>;

  void generate(vec2z size, std::span<int8_t> map, fn_random random,
                log_handler log) noexcept;

  void stretch(vec2z dst_size, std::span<int8_t> dst, vec2z src_size,
               std::span<const int8_t> src,
               sl::index const tunnel_size, sl::index gate_size,
               log_handler log) noexcept;
}

#endif
