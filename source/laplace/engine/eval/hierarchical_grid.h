/*  laplace/engine/eval/hierarchical_grid.h
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

#ifndef laplace_engine_eval_hierarchical_grid_h
#define laplace_engine_eval_hierarchical_grid_h

#include "grid.h"

namespace laplace::engine::eval::hierarchical_grid {
  using astar::fn_heuristic;
  using grid::fn_available;

  struct pivot {
    vec2z block;
    vec2z position;

    bool is_horizontal = false;
    bool is_vertical   = false;

    sl::vector<astar::link> neighbors;
  };

  struct map_info {
    static constexpr auto no_block = vec2z { -1, 0 };

    vec2z                   grid_size;
    sl::whole               grid_stride = 0;
    intval                  grid_scale  = 0;
    std::span<int8_t const> grid_data;
    vec2z                   block_size;
    sl::whole               block_stride = 0;

    fn_available available;

    sl::vector<pivot>                 pivots;
    sl::vector<sl::vector<sl::index>> blocks;
  };

  [[nodiscard]] auto pivot_order(pivot const &a,
                                 pivot const &b) noexcept -> bool;

  [[nodiscard]] auto block_of(vec2z const &   position,
                              map_info const &map) noexcept -> vec2z;

  [[nodiscard]] auto origin_of(vec2z const &   block,
                               map_info const &map) noexcept -> vec2z;

  [[nodiscard]] auto pivots_of(vec2z const &   block,
                               map_info const &map) noexcept
      -> std::span<sl::index const>;

  [[nodiscard]] auto pivots_of_position(vec2z const &   position,
                                        map_info const &map) noexcept
      -> std::span<sl::index const>;

  [[nodiscard]] auto nearest_pivot(vec2z const &   position,
                                   map_info const &map) noexcept
      -> sl::index;

  [[nodiscard]] auto generate(vec2z const  block_size,
                              vec2z const  grid_size,
                              intval const grid_scale,
                              std::span<int8_t const> const grid_data,
                              fn_available const available) noexcept
      -> map_info;

  void process(map_info &map) noexcept;
}

#endif
