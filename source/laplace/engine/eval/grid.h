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
  using op =
      std::function<int8_t(const int8_t dst, const int8_t src)>;

  static constexpr auto op_and =
      [](const int8_t dst, const int8_t src) { return dst & src; };

  static constexpr auto op_or =
      [](const int8_t dst, const int8_t src) { return dst | src; };

  static constexpr auto op_xor =
      [](const int8_t dst, const int8_t src) { return dst ^ src; };

  void merge(const vec2z                   size,
             const std::span<int8_t>       dst,
             const std::span<const int8_t> src,
             const op                      merge_op) noexcept;

  void merge(const vec2z                   dst_size,
             const std::span<int8_t>       dst,
             const vec2z                   src_size,
             const vec2i                   src_offset,
             const std::span<const int8_t> src,
             const op                      merge_op) noexcept;

  using fn_point     = std::function<bool(const vec2z p)>;
  using fn_available = std::function<bool(const int8_t state)>;

  [[nodiscard]] auto trace_line(
      const vec2z    size,
      const vec2z    a,
      const vec2z    b,
      const fn_point point) noexcept -> bool;

  [[nodiscard]] auto neighbors4(
      const sl::index               width,
      const intval                  scale,
      const std::span<const int8_t> map,
      const fn_available            available,
      const sl::index               position,
      const sl::index               n) noexcept -> astar::link;

  [[nodiscard]] auto neighbors8(
      const sl::index               width,
      const intval                  scale,
      const std::span<const int8_t> map,
      const fn_available            available,
      const sl::index               position,
      const sl::index               n) noexcept -> astar::link;

  [[nodiscard]] auto manhattan(
      const sl::index width,
      const intval    scale,
      const sl::index a,
      const sl::index b) noexcept -> intval;

  [[nodiscard]] auto diagonal(
      const sl::index width,
      const intval    scale,
      const sl::index a,
      const sl::index b) noexcept -> intval;

  [[nodiscard]] auto euclidean(
      const sl::index width,
      const intval    scale,
      const sl::index a,
      const sl::index b) noexcept -> intval;

  [[nodiscard]] auto path_exists(
      const sl::index               width,
      const std::span<const int8_t> map,
      const fn_available            available,
      const vec2z                   a,
      const vec2z                   b) noexcept -> bool;

  struct _state {
    astar::_state<true, astar::_node_theta> astar;

    sl::whole width;

    astar::fn_heuristic heuristic;
    astar::fn_neighbors neighbors;
    astar::fn_sight     sight;
  };

  [[nodiscard]] auto path_search_init(
      const vec2z                   size,
      const intval                  scale,
      const std::span<const int8_t> map,
      const fn_available            available,
      const vec2z                   source,
      const vec2z                   destination) noexcept -> _state;

  [[nodiscard]] auto path_search_loop(_state &state) noexcept
      -> astar::status;

  [[nodiscard]] auto path_search_finish(const _state &state) noexcept
      -> sl::vector<vec2z>;

  void convolve(
      const vec2z             size,
      std::span<int8_t>       dst,
      std::span<const int8_t> src,
      const vec2z             fp_size,
      const vec2z             center,
      std::span<const int8_t> footprint) noexcept;

  [[nodiscard]] auto nearest(
      const vec2z                       position,
      const vec2z                       size,
      std::span<const int8_t>           map,
      std::function<bool(const int8_t)> condition =
          [](const int8_t x) { return x <= 0; }) noexcept -> vec2z;
}

#endif
