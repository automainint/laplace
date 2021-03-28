/*  laplace/engine/eval/ee_maze.cpp
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

#include "maze.h"

namespace laplace::engine::eval::maze {
  using std::span, std::function;

  void generate(                   //
      const vec2z            size, //
      const span<intval>     map,  //
      const function<bool()> random) { }

  void stretch(                            //
      const vec2z               dst_size,  //
      const span<uint8_t>       dst,       //
      const vec2z               src_size,  //
      const span<const uint8_t> src,       //
      const size_t              wall_size, //
      const size_t              gate_size) noexcept { }
}
