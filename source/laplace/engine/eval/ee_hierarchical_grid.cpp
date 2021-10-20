/*  laplace/engine/eval/ee_hierarchical_grid.cpp
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

#include "hierarchical_grid.h"

#include "astar.impl.h"
#include "integral.h"
#include <numeric>

namespace laplace::engine::eval::hierarchical_grid {
  using std::span, std::tuple, std::min, std::max, std::lower_bound,
      math::mul_by_elem, std::midpoint;

  auto pivot_order(pivot const &a, pivot const &b) noexcept -> bool {
    return a.block.y() < b.block.y() || a.block.x() < b.block.x() ||
           a.position.y() < b.position.y() ||
           a.position.x() < b.position.x();
  };

  auto block_of(vec2z const &position, map_info const &map) noexcept
      -> vec2z {

    if (map.block_size.x() <= 0 || map.block_size.y() <= 0) {
      error_("Invalid block size.", __FUNCTION__);
      return map.no_block;
    }

    if (!grid::contains(map.grid_area, position))
      return map.no_block;

    auto const p = position - map.grid_area.origin;

    return { p.x() / map.block_size.x(), p.y() / map.block_size.y() };
  }

  auto origin_of(vec2z const &block, map_info const &map) noexcept
      -> vec2z {
    return map.grid_area.origin + mul_by_elem(block, map.block_size);
  }

  auto pivots_of(const vec2z &block, const map_info &map) noexcept
      -> span<const sl::index> {
    const auto n = block.y() * map.block_stride + block.x();

    if (n < 0 || n >= map.blocks.size()) {
      return {};
    }

    return map.blocks[n];
  }

  auto pivots_of_position(const vec2z &   position,
                          const map_info &map) noexcept
      -> span<sl::index const> {

    return pivots_of(block_of(position, map), map);
  }

  auto generate(vec2z const           block_size,
                fn_available const    available,
                intval const          grid_scale,
                grid::rect_area const grid_area) noexcept
      -> map_info {

    if (block_size.x() <= 0 || block_size.y() <= 0) {
      error_("Invalid block size.", __FUNCTION__);
      return {};
    }

    auto map = map_info { .grid_scale   = grid_scale,
                          .grid_area    = grid_area,
                          .block_size   = block_size,
                          .block_stride = (grid_area.width +
                                           block_size.x() - 1) /
                                          block_size.x(),
                          .available = available };

    auto check = [&](vec2z const p) {
      return grid::contains(grid_area, p) &&
             available(grid::value(grid_area, p));
    };

    auto add = [&](pivot &&p) {
      const auto n = p.block.y() * map.block_stride + p.block.x();
      const auto k = map.pivots.size();

      if (p.block.x() > 0 && p.block.y() > 0 && p.is_horizontal &&
          p.is_vertical) {
        map.blocks[n - map.block_stride - 1].emplace_back(k);
      }

      if (p.block.y() > 0 && p.is_horizontal) {
        map.blocks[n - map.block_stride].emplace_back(k);
      }

      if (p.block.x() > 0 && p.is_vertical) {
        map.blocks[n - 1].emplace_back(k);
      }

      map.blocks.back().emplace_back(k);
      map.pivots.emplace_back(p);
    };

    for (sl::index by = 0, y = 0;
         y * grid_area.stride < grid_area.map.size();
         by++, y += block_size.y())
      for (sl::index bx = 0, x = 0; x < grid_area.width;
           bx++, x += block_size.x()) {

        map.blocks.emplace_back();

        const auto x0 = x + grid_area.origin.x();
        const auto y0 = y + grid_area.origin.y();

        const auto x1 = x0 + block_size.x();
        const auto y1 = y0 + block_size.y();

        if (check({ x0, y0 })) {
          add(pivot { .block         = { bx, by },
                      .position      = { x0, y0 },
                      .is_horizontal = true,
                      .is_vertical   = true });
        }

        if (block_size.x() > 1) {
          sl::index i0 = check({ x0 + 1, y0 }) ? 1 : 2;

          for (sl::index i = 2; x + i < grid_area.width; i++) {
            if ((i >= block_size.x() || !check({ x0 + i, y0 })) &&
                check({ x0 + i - 1, y0 })) {
              add(pivot {
                  .block         = { bx, by },
                  .position      = { x0 + midpoint(i0, i - 1), y0 },
                  .is_horizontal = true });
            }

            if (i >= block_size.x())
              break;

            if (!check({ x0 + i, y0 }))
              i0 = i + 1;
          }
        }

        if (block_size.y() > 1) {
          sl::index j0 = check({ x0, y0 + 1 }) ? 1 : 2;

          for (sl::index j = 2;
               (y + j) * grid_area.stride < grid_area.map.size();
               j++) {
            if ((j >= block_size.y() || !check({ x0, y0 + j })) &&
                check({ x0, y0 + j - 1 })) {
              add(pivot {
                  .block       = { bx, by },
                  .position    = { x0, y0 + midpoint(j0, j - 1) },
                  .is_vertical = true });
            }

            if (j >= block_size.y())
              break;

            if (!check({ x0, y0 + j }))
              j0 = j + 1;
          }
        }
      }

    return map;
  }

  auto nearest_pivot(vec2z const &   position,
                     map_info const &map) noexcept -> sl::index {
    auto const block  = block_of(position, map);
    auto const pivots = pivots_of(block, map);
    auto const size   = map.block_size + vec2z { 1, 1 };
    auto const min    = origin_of(block, map);
    auto const max    = min + size;

    auto n      = astar::_invalid_index;
    auto length = intval {};

    for (auto const k : pivots) {
      auto search = grid::path_search_init(
          position, map.pivots[k].position, map.available,
          map.grid_scale, grid::submap(min, max, map.grid_area));

      while (grid::path_search_loop(search) ==
             astar::status::progress) { }

      auto const l = astar::length(search.astar);

      if (n < 0 || l < length) {
        n      = k;
        length = l;
      }
    }

    return n;
  }

  void _process_block(vec2z const block, map_info &map) noexcept {
    const auto n = block.y() * map.block_stride + block.x();

    if (n < 0 || n >= map.blocks.size())
      return;

    auto &indices = map.blocks[n];

    for (sl::index i = 0; i < indices.size(); i++)
      for (sl::index j = i + 1; j < indices.size(); j++) {
        const auto min = origin_of(block, map);
        const auto max = min + map.block_size + vec2z { 1, 1 };

        const auto n0 = indices[i];
        const auto n1 = indices[j];

        auto &a = map.pivots[n0];
        auto &b = map.pivots[n1];

        if (grid::path_exists(
                a.position, b.position, map.available,
                grid::submap(min, max, map.grid_area))) {
          const auto distance = grid::euclidean(
              a.position, b.position, map.grid_scale);

          a.neighbors.emplace_back(astar::link { n1, distance });
          b.neighbors.emplace_back(astar::link { n0, distance });
        }
      }
  }

  void process(map_info &map) noexcept {
    for (sl::index j = 0; j * map.block_stride < map.blocks.size();
         j++)
      for (sl::index i = 0; i < map.block_stride; i++)
        _process_block({ i, j }, map);
  }
}
