/*  laplace/render/defs.h
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

#ifndef laplace_render_defs_h
#define laplace_render_defs_h

#include "../graphics/defs.h"
#include "../math/traits.h"
#include <numbers>

namespace laplace::render {
  using timeval = int64_t;

  using graphics::real;
  using math::realmax_t;

  using graphics::mat3;
  using graphics::mat4;
  using graphics::vec2;
  using graphics::vec3;
  using graphics::vec4;

  using graphics::cref_mat3;
  using graphics::cref_mat4;
  using graphics::cref_vec2;
  using graphics::cref_vec3;
  using graphics::cref_vec4;

  /*  We need highest precision for quaternions.
   */
  using quat      = math::quaternion<realmax_t>;
  using ref_quat  = quat &;
  using cref_quat = const quat &;

  template <typename type_>
  static constexpr auto zero = math::get_zero<type_>();

  template <typename type_>
  static constexpr auto unit = math::get_unit<type_>();
}

#endif
