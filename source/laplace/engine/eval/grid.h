/*  laplace/engine/eval/grid.h
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

#ifndef laplace_engine_eval_grid_h
#define laplace_engine_eval_grid_h

#include "astar.h"

namespace laplace::engine::eval::grid {
  using op = std::function<int8_t(int8_t dst, int8_t src)>;

  constexpr auto op_and(int8_t const dst, int8_t const src) noexcept {
    return dst & src;
  }

  constexpr auto op_or(int8_t const dst, int8_t const src) noexcept {
    return dst | src;
  }

  constexpr auto op_xor(int8_t const dst, int8_t const src) noexcept {
    return dst ^ src;
  }

  constexpr auto is_zero(int8_t const x) noexcept {
    return x == 0;
  }

  void merge(vec2z const                   size,
             std::span<int8_t> const       dst,
             std::span<int8_t const> const src,
             op const                      merge_op) noexcept;

  void merge(vec2z const                   dst_size,
             std::span<int8_t> const       dst,
             vec2z const                   src_size,
             vec2i const                   src_offset,
             std::span<int8_t const> const src,
             op const                      merge_op) noexcept;

  using fn_point     = std::function<bool(const vec2z p)>;
  using fn_available = std::function<bool(int8_t const state)>;

  [[nodiscard]] auto trace_line(vec2z const    a,
                                vec2z const    b,
                                fn_point const point) noexcept
      -> bool;

  struct rect_area {
    std::span<int8_t const> map    = {};
    sl::whole               stride = {};
    sl::whole               width  = {};
    vec2z                   origin = {};
  };

  [[nodiscard]] auto area_of(
      vec2z const size, std::span<int8_t const> const map) noexcept
      -> rect_area;

  [[nodiscard]] auto submap(vec2z const     min,
                            vec2z const     max,
                            rect_area const area) noexcept
      -> rect_area;

  [[nodiscard]] auto index_of(rect_area const area,
                              vec2z const     position,
                              sl::index const invalid = -1) noexcept
      -> sl::index;

  [[nodiscard]] auto point_of(sl::whole const stride,
                              sl::index const n) noexcept -> vec2z;

  [[nodiscard]] auto point_of(rect_area const area,
                              sl::index const n) noexcept -> vec2z;

  [[nodiscard]] auto contains(rect_area const area,
                              vec2z const position) noexcept -> bool;

  [[nodiscard]] auto value(rect_area const area,
                           sl::index const n,
                           int8_t const    invalid = -1) noexcept
      -> int8_t;

  [[nodiscard]] auto value(rect_area const area,
                           vec2z const     position,
                           int8_t const    invalid = -1) noexcept
      -> int8_t;

  [[nodiscard]] auto neighbors4(vec2z const        position,
                                sl::index const    n,
                                fn_available const available,
                                intval const       scale,
                                rect_area const    area) noexcept
      -> astar::link;

  [[nodiscard]] auto neighbors8(vec2z const        position,
                                sl::index const    n,
                                fn_available const available,
                                intval const       scale_ortho,
                                intval const       scale_diagonal,
                                rect_area const    area) noexcept
      -> astar::link;

  [[nodiscard]] auto manhattan(vec2z const  a,
                               vec2z const  b,
                               intval const scale) noexcept -> intval;

  [[nodiscard]] auto diagonal(vec2z const  a,
                              vec2z const  b,
                              intval const scale) noexcept -> intval;

  [[nodiscard]] auto euclidean(vec2z const  a,
                               vec2z const  b,
                               intval const scale) noexcept -> intval;

  [[nodiscard]] auto path_exists(vec2z const        source,
                                 vec2z const        destination,
                                 fn_available const available,
                                 rect_area const    area) noexcept
      -> bool;

  struct _state {
    astar::_state<true, astar::_node_theta> astar;

    vec2z     origin = {};
    sl::whole stride = {};

    astar::fn_heuristic heuristic;
    astar::fn_neighbors neighbors;
    astar::fn_sight     sight;
  };

  [[nodiscard]] auto path_search_init(vec2z const        source,
                                      vec2z const        destination,
                                      fn_available const available,
                                      intval const       scale,
                                      rect_area const area) noexcept
      -> _state;

  [[nodiscard]] auto path_search_loop(_state &state) noexcept
      -> astar::status;

  [[nodiscard]] auto path_search_finish(_state const &state) noexcept
      -> sl::vector<vec2z>;

  void convolve(vec2z const                   size,
                std::span<int8_t> const       dst,
                std::span<int8_t const> const src,
                vec2z const                   fp_size,
                vec2z const                   center,
                std::span<int8_t const> const footprint) noexcept;

  [[nodiscard]] auto nearest(
      vec2z const                       position,
      vec2z const                       size,
      std::span<int8_t const> const     map,
      std::function<bool(int8_t)> const condition =
          [](int8_t const x) { return x <= 0; }) noexcept -> vec2z;
}

#endif
