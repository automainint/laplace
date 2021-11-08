/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_vector_h
#define laplace_engine_tridi_vector_h

#include "../defs.h"

namespace laplace::engine::tridi {
  [[nodiscard]] auto add(vec3i const &a, vec3i const &b) noexcept
      -> vec3i;
  [[nodiscard]] auto sub(vec3i const &a, vec3i const &b) noexcept
      -> vec3i;
  [[nodiscard]] auto mul(vec3i const &v, intval x) noexcept -> vec3i;
  [[nodiscard]] auto div(vec3i const &v, intval x) noexcept -> vec3i;
  [[nodiscard]] auto mul(intval x, vec3i const &v) noexcept -> vec3i;
  [[nodiscard]] auto div(intval x, vec3i const &v) noexcept -> vec3i;
  [[nodiscard]] auto dot(vec3i const &a, vec3i const &b) noexcept
      -> intval;
  [[nodiscard]] auto cross(vec3i const &a, vec3i const &b) noexcept
      -> vec3i;
  [[nodiscard]] auto square_length(vec3i const &v) noexcept -> intval;
  [[nodiscard]] auto length(vec3i const &v) noexcept -> intval;
}

#endif
