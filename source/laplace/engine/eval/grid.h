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
  using op = std::function<int8_t(const int8_t dst, const int8_t src)>;

  static constexpr auto op_and = [](const int8_t dst, const int8_t src) {
    return dst & src;
  };

  static constexpr auto op_or = [](const int8_t dst, const int8_t src) {
    return dst | src;
  };

  static constexpr auto op_xor = [](const int8_t dst, const int8_t src) {
    return dst ^ src;
  };

  void merge(const vec2z size, const std::span<int8_t> dst,
             const std::span<const int8_t> src,
             const op                      merge_op) noexcept;

  void merge(const vec2z dst_size, const std::span<int8_t> dst,
             const vec2z src_size, const vec2i src_offset,
             const std::span<const int8_t> src,
             const op                      merge_op) noexcept;

  using fn_point     = std::function<bool(const vec2z p)>;
  using fn_available = std::function<bool(const int8_t state)>;

  auto trace_line(const vec2z size, const vec2z a, const vec2z b,
                  const fn_point point) -> bool;

  auto neighbors4(const sl::index width, const intval scale,
                  const std::span<const int8_t> map,
                  const fn_available available, const sl::index p)
      -> astar::vlink;

  auto neighbors8(const sl::index width, const intval scale,
                  const std::span<const int8_t> map,
                  const fn_available available, const sl::index p)
      -> astar::vlink;

  auto manhattan(const sl::index width, const intval scale,
                 const sl::index a, const sl::index b) -> intval;

  auto diagonal(const sl::index width, const intval scale,
                const sl::index a, const sl::index b) -> intval;

  auto euclidean(const sl::index width, const intval scale,
                 const sl::index a, const sl::index b) -> intval;

  auto path_exists(const sl::index               width,
                   const std::span<const int8_t> map,
                   const fn_available available, const vec2z a,
                   const vec2z b) -> bool;

  auto path_search(const vec2z size, const intval scale,
                   const std::span<const int8_t> map,
                   const fn_available available, const vec2z a,
                   const vec2z b) -> sl::vector<vec2z>;
}

#endif
