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

  struct _node {
    size_t index  = 0;
    size_t parent = 0;
    intval f      = 0;
    intval g      = 0;
    intval h      = 0;
  };

  template <typename fn_path>
  auto _basic_search(                 //
      const fn_path       gen_path,   //
      const fn_neighbours neighbours, //
      const fn_heuristic  heuristic,  //
      const size_t        source,     //
      const size_t        destination) {

    const auto make_neighs = [&](const size_t index) {
      const auto ns    = neighbours(index);
      auto       nodes = vector<_node>(ns.size());

      for (size_t i = 0; i < ns.size(); i++) {
        nodes[i].index  = ns[i].node;
        nodes[i].g      = ns[i].distance;
        nodes[i].parent = index;
      }

      return nodes;
    };

    const auto cmp_f = [](const _node &a, const intval f) {
      return a.f > f;
    };

    if (source == destination)
      return gen_path({}, 0, source, destination);

    auto open   = vector<_node> {};
    auto closed = vector<_node> {};

    open.emplace_back(_node { .index = source });

    while (!open.empty()) {
      _node q = open.back();
      open.pop_back();

      auto neigs = make_neighs(q.index);

      for (auto &n : neigs) {
        if (n.index == destination) {
          return gen_path(closed, n.parent, source, destination);
        }

        n.g += q.g;
        n.h = heuristic(n.index, destination);
        n.f = n.g + n.h;

        auto i = find_if(open.begin(), open.end(),
                         [index = n.index](const _node &n) {
                           return n.index == index;
                         });

        if (i != open.end() && i->f < n.f) {
          continue;
        }

        auto j = find_if(closed.begin(), closed.end(),
                         [index = n.index](const _node &n) {
                           return n.index == index;
                         });

        if (j != closed.end() && j->f < n.f) {
          continue;
        }

        open.emplace(
            lower_bound(open.begin(), open.end(), n.f, cmp_f), n);
      }

      closed.emplace_back(q);
    }

    return decltype(gen_path({}, 0, 0, 0)) {};
  }

  auto exists(                        //
      const fn_neighbours neighbours, //
      const fn_heuristic  heuristic,  //
      const size_t        source,     //
      const size_t        destination) -> bool {

    const auto gen_path = [](span<const _node> closed, size_t parent,
                             size_t source, size_t destination) -> bool {
      return true;
    };

    return _basic_search(
        gen_path, neighbours, heuristic, source, destination);
  }

  auto search(                        //
      const fn_neighbours neighbours, //
      const fn_heuristic  heuristic,  //
      const size_t        source,     //
      const size_t        destination) -> vuint {

    const auto gen_path = [](span<const _node> closed, size_t parent,
                             size_t source,
                             size_t destination) -> vector<size_t> {
      auto v = vector<size_t> {};

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

    return _basic_search(
        gen_path, neighbours, heuristic, source, destination);
  }
}
