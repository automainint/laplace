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
  using op = std::function<uint8_t(const uint8_t dst, const uint8_t src)>;

  static constexpr auto op_and = [](const uint8_t dst,
                                    const uint8_t src) {
    return dst & src;
  };

  static constexpr auto op_or = [](const uint8_t dst,
                                   const uint8_t src) {
    return dst | src;
  };

  static constexpr auto op_xor = [](const uint8_t dst,
                                    const uint8_t src) {
    return dst ^ src;
  };

  void merge(const vec2z size, const span_byte dst,
             const span_cbyte src, const op merge_op) noexcept;

  void merge(const vec2z dst_size, const span_byte dst,
             const vec2z src_size, const vec2i src_offset,
             const span_cbyte src, const op merge_op) noexcept;

  using fn_point     = std::function<bool(const vec2z p)>;
  using fn_available = std::function<bool(const uint8_t state)>;

  auto trace_line(const vec2z size, const vec2z a, const vec2z b,
                  const fn_point point) -> bool;

  auto neighbors4(const size_t width, const intval scale,
                  const span_cbyte map, const fn_available available,
                  const size_t p) -> astar::vlink;

  auto neighbors8(const size_t width, const intval scale,
                  const span_cbyte map, const fn_available available,
                  const size_t p) -> astar::vlink;

  auto manhattan(const size_t width, const intval scale,
                 const size_t a, const size_t b) -> intval;

  auto diagonal(const size_t width, const intval scale,
                const size_t a, const size_t b) -> intval;

  auto euclidean(const size_t width, const intval scale,
                 const size_t a, const size_t b) -> intval;

  auto path_exists(const size_t width, const span_cbyte map,
                   const fn_available available, const vec2z a,
                   const vec2z b) -> bool;

  auto path_search(const vec2z size, const intval scale,
                   const span_cbyte map, const fn_available available,
                   const vec2z a, const vec2z b) -> sl::vector<vec2z>;
}

#endif
