/*  laplace/engine/eval/ee_pathmap.cpp
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

#include "pathmap.h"

namespace laplace::engine::eval::pathmap {
  using std::span;

  void merge(                         //
      const vec2z               size, //
      const span<uint8_t>       dst,  //
      const span<const uint8_t> src,  //
      const op                  merge_op) noexcept {

    const auto count = size.x() * size.y();

    if (count > dst.size()) {
      error(__FUNCTION__, "Invalid destination size.");
      return;
    }

    if (count > src.size()) {
      error(__FUNCTION__, "Invalid source size.");
      return;
    }

    for (size_t i = 0; i < count; i++) {
      dst[i] = merge_op(dst[i], src[i]);
    }
  }

  void merge(                               //
      const vec2z               dst_size,   //
      const span<uint8_t>       dst,        //
      const vec2z               src_size,   //
      const vec2i               src_offset, //
      const span<const uint8_t> src,        //
      const op                  merge_op) noexcept {

    if (dst_size.x() * dst_size.y() > dst.size()) {
      error(__FUNCTION__, "Invalid destination size.");
      return;
    }

    if (src_size.x() * src_size.y() > src.size()) {
      error(__FUNCTION__, "Invalid source size.");
      return;
    }

    for (size_t y = 0; y < src_size.y(); y++) {
      for (size_t x = 0; x < src_size.x(); x++) {

        const auto n = (src_offset.y() + y) * dst_size.x() +
                       (src_offset.x() + x);

        dst[n] = merge_op(dst[n], src[y * src_size.x() + x]);
      }
    }
  }
}
