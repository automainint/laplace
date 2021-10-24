/*  laplace/engine/eval/astar.impl.h
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

#ifndef laplace_engine_eval_astar_impl_h
#define laplace_engine_eval_astar_impl_h

#include "astar.h"

namespace laplace::engine::eval::astar::impl {
  template <typename _node>
  constexpr void _add_length(_node &n, intval const delta) noexcept {
    n.length += delta;
  }

  template <typename _node>
  [[nodiscard]] constexpr auto _get_parent_length(
      const _node &) noexcept -> intval {
    return {};
  }

  constexpr void _add_length(_node_theta &n,
                             intval const delta) noexcept {
    n.length_parent = delta;
    n.length += delta;
  }

  [[nodiscard]] constexpr auto _get_parent_length(
      _node_theta const &n) noexcept -> intval {
    return n.length_parent;
  }
}

namespace laplace::engine::eval::astar {
  template <bool _nearest, typename _node>
  [[nodiscard]] inline auto init(sl::index const source,
                                 sl::index const destination) noexcept
      -> _state<_nearest, _node> {

    auto s = _state<_nearest, _node> {};

    s.source      = source;
    s.destination = destination;

    s.open.emplace_back();
    s.open.back().index = source;

    return s;
  }

  template <bool _nearest, typename _node>
  [[nodiscard]] inline auto loop(
      fn_sight const          &sight,
      fn_neighbors const      &neighbors,
      fn_heuristic const      &heuristic,
      _state<_nearest, _node> &state) noexcept -> status {

    if (state.open.empty())
      return status::failed;

    _node const q = state.open.back();
    state.open.pop_back();

    auto n = _node {};

    for (sl::index k = 0;; k++) {
      auto const l = neighbors(q.index, k);

      if (l.node == link::skip)
        continue;

      if (l.node == link::invalid)
        break;

      n = _node {};

      n.index  = l.node;
      n.parent = q.index;
      n.length = l.distance;

      const auto is_sight = [&]() {
        return q.parent != _invalid_index && sight(q.parent, n.index);
      };

      if (n.index == state.destination) {
        if (is_sight()) {
          n.parent = q.parent;
        } else {
          state.closed.emplace(
              std::lower_bound(state.closed.begin(),
                               state.closed.end(), q.index,
                               [](_node const &a, sl::index const i) {
                                 return a.index < i;
                               }),
              q);
        }

        state.closed.emplace(
            std::lower_bound(state.closed.begin(), state.closed.end(),
                             n.index,
                             [](_node const &a, sl::index const i) {
                               return a.index < i;
                             }),
            n);

        if constexpr (_nearest) {
          state.nearest  = state.destination;
          state.distance = 0;
        }

        return status::success;
      }

      impl::_add_length(n, q.length);
      n.distance  = heuristic(n.index, state.destination);
      n.estimated = n.length + n.distance;

      auto const j = std::lower_bound(
          state.closed.begin(), state.closed.end(), n.index,
          [](_node const &a, sl::index const i) {
            return a.index < i;
          });

      if (j != state.closed.end() && j->index == n.index &&
          j->estimated < n.estimated) {
        continue;
      }

      auto const i = std::find_if(
          state.open.begin(), state.open.end(),
          [&](const _node &a) { return a.index == n.index; });

      if (i != state.open.end()) {
        if (i->estimated < n.estimated) {
          continue;
        }

        state.open.erase(i);
      }

      if (is_sight()) {
        n.length = heuristic(q.parent, n.index);
        impl::_add_length(n, impl::_get_parent_length(q));
        n.estimated = n.length + n.distance;
        n.parent    = q.parent;
      }

      state.open.emplace(
          std::lower_bound(state.open.begin(), state.open.end(),
                           n.estimated,
                           [](_node const &a, intval const f) {
                             return a.estimated > f;
                           }),
          n);
    }

    if constexpr (_nearest) {
      if (state.nearest == _invalid_index) {
        state.nearest  = q.index;
        state.distance = heuristic(q.index, state.destination);
      } else if (q.distance < state.distance) {
        state.nearest  = q.index;
        state.distance = q.distance;
      }
    }

    state.closed.emplace(
        std::lower_bound(state.closed.begin(), state.closed.end(),
                         q.index,
                         [](_node const &a, sl::index const i) {
                           return a.index < i;
                         }),
        q);

    return status::progress;
  }

  template <typename _node>
  [[nodiscard]] inline auto finish(
      std::span<_node const> const closed,
      sl::index const              source,
      sl::index const destination) noexcept -> sl::vector<sl::index> {

    auto path    = sl::vector<sl::index> {};
    auto current = destination;

    while (current != source) {
      path.emplace_back(current);

      auto const i = std::lower_bound(
          closed.begin(), closed.end(), current,
          [](_node const &n, sl::index const i) {
            return n.index < i;
          });

      if (i == closed.end() || i->index != current)
        return {};

      current = i->parent;
    }

    path.emplace_back(current);

    auto v = sl::vector<sl::index>(path.size());

    for (sl::index i = 0, j = path.size() - 1; i < v.size();
         i++, j--) {
      v[i] = path[j];
    }

    return v;
  }

  template <bool _nearest, typename _node>
  inline auto length(_state<_nearest, _node> const &state) noexcept
      -> intval {
    auto const i = std::lower_bound(
        state.closed.begin(), state.closed.end(), state.destination,
        [](_node const &n, sl::index const i) {
          return n.index < i;
        });

    if (i == state.closed.end() || i->index != state.destination)
      return -1;

    return i->length;
  }
}

#endif
