/*  laplace/engine/eval/ee_astar.cpp
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

#include "astar.h"

namespace laplace::engine::eval::astar {
  using std::find_if, std::lower_bound, std::vector, std::span;

  static constexpr sl::index _invalid_index = -1;

  struct _basic_node {
    sl::index index  = _invalid_index;
    sl::index parent = _invalid_index;
    intval    f      = 0;
    intval    g      = 0;

    constexpr void add_g(const intval delta) {
      g += delta;
    }

    constexpr auto get_parent_g() const -> intval {
      return {};
    }
  };

  struct _node_theta {
    sl::index index    = _invalid_index;
    sl::index parent   = _invalid_index;
    intval    f        = 0;
    intval    g        = 0;
    intval    g_parent = 0;

    constexpr void add_g(const intval delta) {
      g_parent = delta;
      g += delta;
    }

    constexpr auto get_parent_g() const -> intval {
      return g_parent;
    }
  };

  template <typename _node, typename fn_path, typename fn_sight>
  auto _basic_search(const fn_path gen_path, const fn_sight sight,
                     const fn_neighbors neighbors,
                     const fn_heuristic heuristic,
                     const sl::index    source,
                     const sl::index    destination) {

    const auto make_neighs = [&](const sl::index index) {
      const auto ns    = neighbors(index);
      auto       nodes = vector<_node>(ns.size());

      for (sl::index i = 0; i < ns.size(); i++) {
        nodes[i].index  = ns[i].node;
        nodes[i].g      = ns[i].distance;
        nodes[i].parent = index;
      }

      return nodes;
    };

    const auto cmp_f = [](const _node &a, const intval f) {
      return a.f > f;
    };

    const auto cmp_index = [](const sl::index index) {
      return [index](const _node &a) {
        return a.index == index;
      };
    };

    if (source == destination)
      return gen_path({}, 0, source, destination);

    auto open   = vector<_node> {};
    auto closed = vector<_node> {};

    open.emplace_back();
    open.back().index = source;

    while (!open.empty()) {
      _node q = open.back();
      open.pop_back();

      auto neigs = make_neighs(q.index);

      for (auto &n : neigs) {
        if (n.index == destination) {
          closed.emplace_back(q);
          return gen_path(closed, n.parent, source, destination);
        }

        n.add_g(q.g);
        n.f = n.g + heuristic(n.index, destination);

        auto j = find_if(
            closed.begin(), closed.end(), cmp_index(n.index));

        if (j != closed.end() && j->f < n.f) {
          continue;
        }

        auto i = find_if(open.begin(), open.end(), cmp_index(n.index));

        if (i != open.end()) {
          if (i->f < n.f) {
            continue;
          }

          open.erase(i);
        }

        if (sight(q.parent, n.index)) {
          n.g = heuristic(q.parent, n.index);
          n.add_g(q.get_parent_g());
          n.f      = n.g + heuristic(n.index, destination);
          n.parent = q.parent;
        }

        open.emplace(
            lower_bound(open.begin(), open.end(), n.f, cmp_f), n);
      }

      closed.emplace_back(q);
    }

    return decltype(gen_path({}, 0, 0, 0)) {};
  }

  template <typename _node>
  auto _basic_path(span<const _node> closed, sl::index parent,
                   sl::index source, sl::index destination)
      -> vector<sl::index> {
    auto v = vector<sl::index> {};

    if (source == destination) {
      v.emplace_back(destination);
      return v;
    }

    v.emplace_back(parent);
    v.emplace_back(destination);

    while (parent != source) {
      auto i = find_if(closed.begin(), closed.end(),
                       [index = parent](const _node &n) {
                         return n.index == index;
                       });

      if (i == closed.end()) {
        return {};
      }

      v.emplace(v.begin(), i->parent);
      parent = i->parent;
    }

    return v;
  };

  auto exists(const fn_neighbors neighbors,
              const fn_heuristic heuristic, const sl::index source,
              const sl::index destination) -> bool {

    const auto gen_path = [](span<const _basic_node> closed,
                             sl::index parent, sl::index source,
                             sl::index destination) -> bool {
      return true;
    };

    const auto sight = [](const sl::index a, const sl::index b) -> bool {
      return false;
    };

    return _basic_search<_basic_node>(
        gen_path, sight, neighbors, heuristic, source, destination);
  }

  auto search(const fn_neighbors neighbors,
              const fn_heuristic heuristic, const sl::index source,
              const sl::index destination) -> sl::vector<sl::index> {

    const auto sight = [](const sl::index a, const sl::index b) -> bool {
      return false;
    };

    return _basic_search<_basic_node>(_basic_path<_basic_node>, sight,
                                      neighbors, heuristic, source,
                                      destination);
  }

  auto search_theta(const fn_neighbors neighbors,
                    const fn_heuristic heuristic, const fn_sight sight,
                    const sl::index source, const sl::index destination)
      -> sl::vector<sl::index> {

    return _basic_search<_node_theta>(_basic_path<_node_theta>, sight,
                                      neighbors, heuristic, source,
                                      destination);
  }
}
