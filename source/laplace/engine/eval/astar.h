/*  laplace/engine/eval/astar.h
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
  enum class status : sl::index { progress, success, failed };

  static constexpr sl::index _invalid_index = -1;

  template <bool _nearest, typename _node>
  struct _state {
    sl::index source      = _invalid_index;
    sl::index destination = _invalid_index;

    sl::vector<_node> open;
    sl::vector<_node> closed;
  };

  template <typename _node>
  struct _state<true, _node> : _state<false, _node> {
    sl::index nearest  = _invalid_index;
    intval    distance = -1;
  };

  struct _basic_node {
    sl::index index  = _invalid_index;
    sl::index parent = _invalid_index;

    /*  Actual length from source to current node.
     */
    intval length = {};

    /*  Distance from current node to destination.
     */
    intval distance = {};

    /*  Estimated length from source to destination.
     */
    intval estimated = {};
  };

  struct _node_theta : _basic_node {
    intval length_parent = {};
  };

  struct link {
    enum node_value : sl::index { invalid = -1, skip = -2 };
    sl::index node     = invalid;
    intval    distance = {};
  };

  using fn_sight =
      std::function<bool(const sl::index a, const sl::index b)>;

  using fn_neighbors =
      std::function<link(const sl::index node, const sl::index n)>;

  using fn_heuristic =
      std::function<intval(const sl::index a, const sl::index b)>;

  template <bool _nearest, typename _node>
  [[nodiscard]] inline auto init(
      const sl::index source, const sl::index destination) noexcept
      -> _state<_nearest, _node>;

  template <bool _nearest, typename _node>
  [[nodiscard]] inline auto loop(
      const fn_sight           sight,
      const fn_neighbors       neighbors,
      const fn_heuristic       heuristic,
      _state<_nearest, _node> &state) noexcept -> status;

  template <typename _node>
  [[nodiscard]] inline auto finish(
      std::span<const _node> closed,
      sl::index              source,
      sl::index destination) noexcept -> sl::vector<sl::index>;
}

#endif
