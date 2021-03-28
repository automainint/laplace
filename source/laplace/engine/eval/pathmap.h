/*  laplace/engine/eval/pathmap.h
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

#ifndef laplace_engine_eval_pathmap_h
#define laplace_engine_eval_pathmap_h

#include "../defs.h"

namespace laplace::engine::eval::pathmap {
  enum pathpoint : uint8_t { walkable = 0, blocked };

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

  void merge(                              //
      const vec2z                    size, //
      const std::span<uint8_t>       dst,  //
      const std::span<const uint8_t> src,  //
      const op                       merge_op) noexcept;

  void merge(                                    //
      const vec2z                    dst_size,   //
      const std::span<uint8_t>       dst,        //
      const vec2z                    src_size,   //
      const vec2i                    src_offset, //
      const std::span<const uint8_t> src,        //
      const op                       merge_op) noexcept;
}

#endif
