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

  static constexpr auto op_and =
      [](int8_t const dst, int8_t const src) { return dst & src; };

  static constexpr auto op_or =
      [](int8_t const dst, int8_t const src) { return dst | src; };

  static constexpr auto op_xor =
      [](int8_t const dst, int8_t const src) { return dst ^ src; };

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

  [[nodiscard]] auto neighbors4(sl::whole const               width,
                                sl::whole const               stride,
                                intval const                  scale,
                                std::span<int8_t const> const map,
                                fn_available const available,
                                sl::index const    position,
                                sl::index const    n) noexcept
      -> astar::link;

  [[nodiscard]] auto neighbors8(sl::whole const               width,
                                sl::whole const               stride,
                                intval const                  scale,
                                std::span<int8_t const> const map,
                                fn_available const available,
                                sl::index const    position,
                                sl::index const    n) noexcept
      -> astar::link;

  [[nodiscard]] auto manhattan(sl::whole const stride,
                               intval const    scale,
                               sl::index const a,
                               sl::index const b) noexcept -> intval;

  [[nodiscard]] auto diagonal(sl::whole const stride,
                              intval const    scale,
                              sl::index const a,
                              sl::index const b) noexcept -> intval;

  [[nodiscard]] auto euclidean(sl::whole const stride,
                               intval const    scale,
                               sl::index const a,
                               sl::index const b) noexcept -> intval;

  /*  Returns true if path exists with specified stride.
   */
  [[nodiscard]] auto path_exists(sl::whole const               width,
                                 sl::whole const               stride,
                                 std::span<int8_t const> const map,
                                 fn_available const available,
                                 vec2z const        source,
                                 vec2z const destination) noexcept
      -> bool;

  /*  Returns true if path exists.
   */
  [[nodiscard]] auto path_exists(sl::whole const               width,
                                 std::span<int8_t const> const map,
                                 fn_available const available,
                                 vec2z const        source,
                                 vec2z const destination) noexcept
      -> bool;

  /*  Returns true if path exists in bounds [min; max).
   */
  [[nodiscard]] auto path_exists(sl::whole const               width,
                                 vec2z const                   min,
                                 vec2z const                   max,
                                 std::span<int8_t const> const map,
                                 fn_available const available,
                                 vec2z const        source,
                                 vec2z const destination) noexcept
      -> bool;

  struct _state {
    astar::_state<true, astar::_node_theta> astar;

    vec2z     origin = {};
    sl::whole stride = {};

    astar::fn_heuristic heuristic;
    astar::fn_neighbors neighbors;
    astar::fn_sight     sight;
  };

  /*  Search in entire map with specified stride and origin offset.
   */
  [[nodiscard]] auto path_search_init(
      vec2z const                   size,
      vec2z const                   origin,
      sl::whole const               stride,
      intval const                  scale,
      std::span<int8_t const> const map,
      fn_available const            available,
      vec2z const                   source,
      vec2z const                   destination) noexcept -> _state;

  /*  Search in entire map.
   */
  [[nodiscard]] auto path_search_init(
      vec2z const                   size,
      intval const                  scale,
      std::span<int8_t const> const map,
      fn_available const            available,
      vec2z const                   source,
      vec2z const                   destination) noexcept -> _state;

  /*  Search in bounds [min; max).
   */
  [[nodiscard]] auto path_search_init(
      vec2z const                   size,
      vec2z const                   min,
      vec2z const                   max,
      intval const                  scale,
      std::span<int8_t const> const map,
      fn_available const            available,
      vec2z const                   source,
      vec2z const                   destination) noexcept -> _state;

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
