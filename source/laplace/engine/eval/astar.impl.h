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
  constexpr void _set_parent_g(_node &n, const intval delta) noexcept {
    n.g += delta;
  }

  template <typename _node>
  constexpr auto _get_parent_g(const _node &n) noexcept -> intval {
    return {};
  }

  constexpr void _set_parent_g(_node_theta &n,
                               const intval delta) noexcept {
    n.g_parent = delta;
    n.g += delta;
  }

  constexpr auto _get_parent_g(const _node_theta &n) noexcept
      -> intval {
    return n.g_parent;
  }
}

namespace laplace::engine::eval::astar {
  template <bool _nearest, typename _node>
  inline auto init(const sl::index source,
                   const sl::index destination) noexcept
      -> _state<_nearest, _node> {

    auto s = _state<_nearest, _node> {};

    s.source      = source;
    s.destination = destination;

    s.open.emplace_back();
    s.open.back().index = source;

    return s;
  }

  template <bool _nearest, typename _node>
  inline auto loop(const fn_sight sight, const fn_neighbors neighbors,
                   const fn_heuristic       heuristic,
                   _state<_nearest, _node> &state) noexcept -> status {

    const auto make_neighs = [&](const sl::index index) {
      const auto ns    = neighbors(index);
      auto       nodes = sl::vector<_node>(ns.size());

      for (sl::index i = 0; i < ns.size(); i++) {
        nodes[i].index  = ns[i].node;
        nodes[i].g      = ns[i].distance;
        nodes[i].parent = index;
      }

      return nodes;
    };

    if (state.open.empty()) {
      return status::failed;
    }

    _node q = state.open.back();
    state.open.pop_back();

    auto neigs = make_neighs(q.index);

    for (auto &n : neigs) {
      auto is_sight = [&]() {
        return q.parent != _invalid_index && sight(q.parent, n.index);
      };

      if (n.index == state.destination) {
        if (is_sight()) {
          n.parent = q.parent;
        } else {
          state.closed.emplace(
              std::lower_bound(state.closed.begin(),
                               state.closed.end(), q.index,
                               [](const _node &a, const sl::index i) {
                                 return a.index < i;
                               }),
              q);
        }

        state.closed.emplace(
            std::lower_bound(state.closed.begin(), state.closed.end(),
                             n.index,
                             [](const _node &a, const sl::index i) {
                               return a.index < i;
                             }),
            n);

        if constexpr (_nearest) {
          state.nearest  = state.destination;
          state.distance = 0;
        }

        return status::success;
      }

      impl::_set_parent_g(n, q.g);
      n.f = n.g + heuristic(n.index, state.destination);

      auto j = std::lower_bound(state.closed.begin(),
                                state.closed.end(), n.index,
                                [](const _node &a, const sl::index i) {
                                  return a.index < i;
                                });

      if (j != state.closed.end() && j->index == n.index && j->f < n.f) {
        continue;
      }

      auto i = std::find_if(state.open.begin(), state.open.end(),
                            [&](const _node &a) {
                              return a.index == n.index;
                            });

      if (i != state.open.end()) {
        if (i->f < n.f) {
          continue;
        }

        state.open.erase(i);
      }

      if (is_sight()) {
        n.g = heuristic(q.parent, n.index);
        impl::_set_parent_g(n, impl::_get_parent_g(q));
        n.f      = n.g + heuristic(n.index, state.destination);
        n.parent = q.parent;
      }

      state.open.emplace(
          std::lower_bound(state.open.begin(), state.open.end(), n.f,
                           [](const _node &a, const intval f) {
                             return a.f > f;
                           }),
          n);
    }

    if constexpr (_nearest) {
      const auto distance = heuristic(q.index, state.destination);

      if (state.nearest == _invalid_index || distance < state.distance) {
        state.nearest  = q.index;
        state.distance = distance;
      }
    }

    state.closed.emplace(
        std::lower_bound(state.closed.begin(), state.closed.end(),
                         q.index,
                         [](const _node &a, const sl::index i) {
                           return a.index < i;
                         }),
        q);

    return status::progress;
  }

  template <typename _node>
  inline auto finish(std::span<const _node> closed, sl::index source,
                     sl::index destination) noexcept
      -> sl::vector<sl::index> {

    auto path    = sl::vector<sl::index> {};
    auto current = destination;

    while (current != source) {
      path.emplace_back(current);

      auto i = std::lower_bound(closed.begin(), closed.end(), current,
                                [](const _node &n, const sl::index i) {
                                  return n.index < i;
                                });

      if (i == closed.end() || i->index != current) {
        return {};
      }

      current = i->parent;
    }

    path.emplace_back(current);

    auto v = sl::vector<sl::index>(path.size());

    for (sl::index i = 0, j = path.size() - 1; i < v.size(); i++, j--) {
      v[i] = path[j];
    }

    return v;
  };
}

#endif
