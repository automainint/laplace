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
  using grid::fn_available;

  struct pivot {
    vec2z block;
    vec2z position;

    bool is_horizontal = false;
    bool is_vertical   = false;

    sl::vector<sl::index> neighbors;
  };

  struct map_info {
    static constexpr auto no_block = vec2z { -1, 0 };

    vec2z                   grid_size;
    std::span<const int8_t> grid_data;
    vec2z                   block_size;
    sl::whole               block_stride = 0;
    fn_available            available;

    sl::vector<pivot>                 pivots;
    sl::vector<sl::vector<sl::index>> blocks;
  };

  [[nodiscard]] auto pivot_order(const pivot &a,
                                 const pivot &b) noexcept -> bool;

  [[nodiscard]] auto block_of(const vec2z &   position,
                              const map_info &map) noexcept -> vec2z;

  [[nodiscard]] auto pivots_of(const vec2z &   block,
                               const map_info &map) noexcept
      -> std::span<const sl::index>;

  [[nodiscard]] auto pivots_of_position(const vec2z &   position,
                                        const map_info &map) noexcept
      -> std::span<const sl::index>;

  [[nodiscard]] auto generate(const vec2z block_size,
                              const vec2z grid_size,
                              const std::span<const int8_t> grid_data,
                              const fn_available available) noexcept
      -> map_info;

  void process(const vec2z source,
               const vec2z destination,
               map_info &  map) noexcept;
}

#endif
