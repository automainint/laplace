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

namespace laplace::engine::eval::hierarchical_grid {
  using std::span, std::tuple, std::min, std::max, std::lower_bound,
      math::mul_by_elem;

  auto pivot_order(const pivot &a, const pivot &b) noexcept -> bool {
    return a.block.y() < b.block.y() || a.block.x() < b.block.x() ||
           a.position.y() < b.position.y() ||
           a.position.x() < b.position.x();
  };

  auto block_of(const vec2z &position, const map_info &map) noexcept
      -> vec2z {

    if (map.block_size.x() <= 0 || map.block_size.y() <= 0) {
      error_("Invalid block size.", __FUNCTION__);
      return map.no_block;
    }

    if (position.x() < 0 || position.x() >= map.grid_size.x() ||
        position.y() < 0 || position.y() >= map.grid_size.y()) {
      return map.no_block;
    }

    return { position.x() / map.block_size.x(),
             position.y() / map.block_size.y() };
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
      -> span<const sl::index> {

    return pivots_of(block_of(position, map), map);
  }

  auto _generate_basic(const vec2z              block_size,
                       const vec2z              grid_size,
                       const sl::whole          stride,
                       const span<const int8_t> grid_data,
                       const fn_available       available) noexcept
      -> map_info {

    if (block_size.x() <= 0 || block_size.y() <= 0) {
      error_("Invalid block size.", __FUNCTION__);
      return {};
    }

    auto map = map_info { .grid_size    = grid_size,
                          .grid_data    = grid_data,
                          .block_size   = block_size,
                          .block_stride = grid_size.x() /
                                          block_size.x(),
                          .available = available };

    auto index_of = [&](const vec2z p) {
      return p.y() * stride + p.x();
    };

    auto check = [&](const sl::index n) {
      return available(grid_data[n]);
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

    for (sl::index by = 0, y0 = 0; y0 < grid_size.y();
         by++, y0 += block_size.y())
      for (sl::index bx = 0, x0 = 0; x0 < grid_size.x();
           bx++, x0 += block_size.x()) {

        map.blocks.emplace_back();

        const auto x1 = x0 + block_size.x();
        const auto y1 = y0 + block_size.y();

        const auto n0 = index_of({ x0, y0 });

        if (check(index_of({ x0, y0 }))) {
          add(pivot { .block         = { bx, by },
                      .position      = { x0, y0 },
                      .is_horizontal = true,
                      .is_vertical   = true });
        }

        if (block_size.x() > 1)
          for (sl::index i = 2; x0 + i < grid_size.x(); i++) {
            const auto n = n0 + i;

            if ((i >= block_size.x() || !check(n)) && check(n - 1)) {
              add(pivot { .block         = { bx, by },
                          .position      = { x0 + i - 1, y0 },
                          .is_horizontal = true });
            }

            if (i >= block_size.x()) {
              break;
            }
          }

        if (block_size.y() > 1)
          for (sl::index j = 2; y0 + j < grid_size.y(); j++) {
            const auto n = n0 + j * stride;

            if ((j >= block_size.y() || !check(n)) &&
                check(n - stride)) {
              add(pivot { .block       = { bx, by },
                          .position    = { x0, y0 + j - 1 },
                          .is_vertical = true });
            }

            if (j >= block_size.y()) {
              break;
            }
          }
      }

    return map;
  }

  auto generate(const vec2z              block_size,
                const vec2z              grid_size,
                const span<const int8_t> grid_data,
                const fn_available available) noexcept -> map_info {

    return _generate_basic(block_size, grid_size, grid_size.x(),
                           grid_data, available);
  }

  void _process_block(const vec2z block, map_info &map) noexcept {
    auto &indices =
        map.blocks[block.y() * map.block_stride + block.x()];

    for (sl::index i = 0; i < indices.size(); i++)
      for (sl::index j = i + 1; j < indices.size(); j++) {
        const auto min = mul_by_elem(block, map.block_size);
        const auto max = min + map.block_size + vec2z { 1, 1 };

        const auto n0 = indices[i];
        const auto n1 = indices[j];

        auto &a = map.pivots[n0];
        auto &b = map.pivots[n1];

        if (grid::path_exists(map.grid_size.x(), min, max,
                              map.grid_data, map.available,
                              a.position, b.position)) {
          a.neighbors.emplace_back(n1);
          b.neighbors.emplace_back(n0);
        }
      }
  }

  void process(const vec2z source,
               const vec2z destination,
               map_info &  map) noexcept {
    const auto src_block = block_of(source, map);
    const auto dst_block = block_of(destination, map);

    _process_block(src_block, map);
  }
}
