/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_EVAL_GRID_H
#define LAPLACE_ENGINE_EVAL_GRID_H

#include "astar.h"

namespace laplace::engine::eval::grid {
  using op = std::function<int8_t(int8_t dst, int8_t src)>;

  [[nodiscard]] constexpr auto op_and(int8_t dst,
                                      int8_t src) noexcept {
    return static_cast<int8_t>(static_cast<uint8_t>(dst) &
                               static_cast<uint8_t>(src));
  }

  [[nodiscard]] constexpr auto op_or(int8_t dst,
                                     int8_t src) noexcept {
    return static_cast<int8_t>(static_cast<uint8_t>(dst) |
                               static_cast<uint8_t>(src));
  }

  [[nodiscard]] constexpr auto op_xor(int8_t dst,
                                      int8_t src) noexcept {
    return static_cast<int8_t>(static_cast<uint8_t>(dst) ^
                               static_cast<uint8_t>(src));
  }

  [[nodiscard]] constexpr auto is_zero(int8_t x) noexcept {
    return x == 0;
  }

  void merge(vec2z size, std::span<int8_t> dst,
             std::span<int8_t const> src, op merge_op) noexcept;

  void merge(vec2z dst_size, std::span<int8_t> dst, vec2z src_size,
             vec2i src_offset, std::span<int8_t const> src,
             op merge_op) noexcept;

  using fn_point     = std::function<bool(vec2z p)>;
  using fn_available = std::function<bool(int8_t state)>;

  [[nodiscard]] auto trace_line(vec2z a, vec2z b,
                                fn_point const &point) noexcept
      -> bool;

  struct rect_area {
    std::span<int8_t const> map    = {};
    sl::whole               stride = {};
    sl::whole               width  = {};
    vec2z                   origin = {};
  };

  [[nodiscard]] auto area_of(vec2z                   size,
                             std::span<int8_t const> map) noexcept
      -> rect_area;

  [[nodiscard]] auto submap(vec2z min, vec2z max,
                            rect_area area) noexcept -> rect_area;

  [[nodiscard]] auto index_of(rect_area area, vec2z position,
                              sl::index invalid = -1) noexcept
      -> sl::index;

  [[nodiscard]] auto point_of(sl::whole stride, sl::index n) noexcept
      -> vec2z;

  [[nodiscard]] auto point_of(rect_area area, sl::index n) noexcept
      -> vec2z;

  [[nodiscard]] auto contains(rect_area area, vec2z position) noexcept
      -> bool;

  [[nodiscard]] auto value(rect_area area, sl::index n,
                           int8_t invalid = -1) noexcept -> int8_t;

  [[nodiscard]] auto value(rect_area area, vec2z position,
                           int8_t invalid = -1) noexcept -> int8_t;

  [[nodiscard]] auto neighbors4(vec2z position, sl::index n,
                                fn_available available, intval scale,
                                rect_area area) noexcept
      -> astar::link;

  [[nodiscard]] auto neighbors8(vec2z position, sl::index n,
                                fn_available available,
                                intval       scale_ortho,
                                intval       scale_diagonal,
                                rect_area    area) noexcept
      -> astar::link;

  [[nodiscard]] auto manhattan(vec2z a, vec2z b,
                               intval scale) noexcept -> intval;

  [[nodiscard]] auto diagonal(vec2z a, vec2z b, intval scale) noexcept
      -> intval;

  [[nodiscard]] auto euclidean(vec2z a, vec2z b,
                               intval scale) noexcept -> intval;

  [[nodiscard]] auto path_exists(vec2z source, vec2z destination,
                                 fn_available available,
                                 rect_area    area) noexcept -> bool;

  struct _state {
    astar::_state<true, astar::_node_theta> astar;

    vec2z     origin = {};
    sl::whole stride = {};

    astar::fn_heuristic heuristic;
    astar::fn_neighbors neighbors;
    astar::fn_sight     sight;
  };

  [[nodiscard]] auto path_search_init(vec2z source, vec2z destination,
                                      fn_available available,
                                      intval       scale,
                                      rect_area    area) noexcept
      -> _state;

  [[nodiscard]] auto path_search_loop(_state &state) noexcept
      -> astar::status;

  [[nodiscard]] auto path_search_finish(_state const &state) noexcept
      -> sl::vector<vec2z>;

  void convolve(vec2z size, std::span<int8_t> dst,
                std::span<int8_t const> src, vec2z fp_size,
                vec2z                   center,
                std::span<int8_t const> footprint) noexcept;

  [[nodiscard]] auto nearest(
      vec2z position, vec2z size, std::span<int8_t const> map,
      std::function<bool(int8_t)> condition = [](int8_t const x) {
        return x <= 0;
      }) noexcept -> vec2z;
}

#endif
