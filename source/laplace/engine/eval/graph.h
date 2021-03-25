/*  laplace/engine/eval/graph.h
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

#ifndef laplace_engine_eval_graph_h
#define laplace_engine_eval_graph_h

#include "../defs.h"

namespace laplace::engine::eval {
  using pt = uint8_t;

  auto path_exists(                   //
      const vec2z               size, //
      const std::span<const pt> map,  //
      const vec2z               a,    //
      const vec2z               b) noexcept -> bool;

  auto find_path(                     //
      const vec2z               size, //
      const std::span<const pt> map,  //
      const vec2z               a,    //
      const vec2z               b) -> std::vector<vec2z>;

  enum merge_op { merge_and, merge_or, merge_xor };

  void merge(                         //
      const vec2z               size, //
      const std::span<intval>   dst,  //
      const std::span<const pt> src,  //
      const merge_op            op) noexcept;

  void merge(                               //
      const vec2z               dst_size,   //
      const std::span<pt>       dst,        //
      const vec2z               src_size,   //
      const vec2i               src_offset, //
      const std::span<const pt> src,        //
      const merge_op            op) noexcept;

  void stretch(                            //
      const vec2z               dst_size,  //
      const std::span<pt>       dst,       //
      const vec2z               src_size,  //
      const std::span<const pt> src,       //
      const size_t              wall_size, //
      const size_t              gate_size) noexcept;

  void gen_maze(                        //
      const vec2z                 size, //
      const std::span<intval>     map,  //
      const std::function<bool()> random);
}

#endif
