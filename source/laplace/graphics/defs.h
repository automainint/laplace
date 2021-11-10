/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_graphics_defs_h
#define laplace_graphics_defs_h

#include "../core/slib.h"
#include "../math/matrix.h"
#include "../math/quaternion.h"
#include "../math/vector.h"

namespace laplace::graphics {
  using real = float;

  using vec2 = math::vector<2, real>;
  using vec3 = math::vector<3, real>;
  using vec4 = math::vector<4, real>;
  using mat3 = math::matrix<3, 3, real>;
  using mat4 = math::matrix<4, 4, real>;

  using ref_vec2 = vec2 &;
  using ref_vec3 = vec3 &;
  using ref_vec4 = vec4 &;
  using ref_mat3 = mat3 &;
  using ref_mat4 = mat4 &;

  using cref_vec2 = vec2 const &;
  using cref_vec3 = vec3 const &;
  using cref_vec4 = vec4 const &;
  using cref_mat3 = mat3 const &;
  using cref_mat4 = mat4 const &;
}

#endif
