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

#ifndef laplace_engine_eval_astar_h
#define laplace_engine_eval_astar_h

#include "../defs.h"

namespace laplace::engine::eval::astar {
  struct link {
    size_t node     = 0;
    intval distance = 0;
  };

  using vlink = std::vector<link>;

  using fn_neighbours = std::function<vlink(const size_t node)>;
  using fn_heuristic =
      std::function<intval(const size_t a, const size_t b)>;

  auto exists(                        //
      const fn_neighbours neighbours, //
      const fn_heuristic  heuristic,  //
      const size_t        source,     //
      const size_t        destination) -> bool;

  auto search(                        //
      const fn_neighbours neighbours, //
      const fn_heuristic  heuristic,  //
      const size_t        source,     //
      const size_t        destination) -> vuint;
}

#endif
