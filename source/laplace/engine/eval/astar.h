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
    sl::index node     = 0;
    intval    distance = 0;
  };

  using vlink = sl::vector<link>;

  using fn_sight =
      std::function<bool(const sl::index a, const sl::index b)>;

  using fn_neighbors = std::function<vlink(const sl::index node)>;

  using fn_heuristic =
      std::function<intval(const sl::index a, const sl::index b)>;

  auto exists(const fn_neighbors neighbors,
              const fn_heuristic heuristic, const sl::index source,
              const sl::index destination) -> bool;

  auto search(const fn_neighbors neighbors,
              const fn_heuristic heuristic, const sl::index source,
              const sl::index destination) -> sl::vector<sl::index>;

  auto search_theta(const fn_neighbors neighbors,
                    const fn_heuristic heuristic, const fn_sight sight,
                    const sl::index source, const sl::index destination)
      -> sl::vector<sl::index>;
}

#endif
