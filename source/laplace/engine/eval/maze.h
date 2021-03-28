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

#include "pathmap.h"

namespace laplace::engine::eval::maze {
  void generate(                        //
      const vec2z                 size, //
      const std::span<intval>     map,  //
      const std::function<bool()> random);

  void stretch(                                 //
      const vec2z                    dst_size,  //
      const std::span<uint8_t>       dst,       //
      const vec2z                    src_size,  //
      const std::span<const uint8_t> src,       //
      const size_t                   wall_size, //
      const size_t                   gate_size) noexcept;
}

#endif
