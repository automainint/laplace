/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_EVAL_INTEGRAL_H
#define LAPLACE_ENGINE_EVAL_INTEGRAL_H

#include "../defs.h"

namespace laplace::engine::eval {
  auto constant_scale() noexcept -> intval;

  /*  x + y
   */
  auto add(intval x, intval y) noexcept -> intval;
  auto op_add(intval x, intval y) noexcept -> intval;

  /*  x - y
   */
  auto sub(intval x, intval y) noexcept -> intval;
  auto op_sub(intval x, intval y) noexcept -> intval;

  /*  x / y
   */
  auto div(intval x, intval y) noexcept -> intval;
  auto op_div(intval x, intval y) noexcept -> intval;

  /*  (x / scale) / (y / scale) * scale
   */
  auto div(intval x, intval y, intval scale) noexcept -> intval;

  /*  x * y
   */
  auto mul(intval x, intval y) noexcept -> intval;
  auto op_mul(intval x, intval y) noexcept -> intval;

  /*  (x / scale) * (y / scale) * scale
   */
  auto mul(intval x, intval y, intval scale) noexcept -> intval;

  /*  x * x
   */
  auto sqr(intval x) noexcept -> intval;

  /*  (x / scale) * (x / scale) * scale
   */
  auto sqr(intval x, intval scale) noexcept -> intval;

  /*  (x + y) / divisor
   */
  auto div_sum(intval x, intval y, intval divisor) noexcept -> intval;

  /*  (x + y + z) / divisor
   */
  auto div_sum(intval x, intval y, intval z, intval divisor) noexcept
      -> intval;

  /*  2.718281828459045
   */
  auto e(intval scale) noexcept -> intval;

  /*  1.4426950408889634
   */
  auto log2e(intval scale) noexcept -> intval;

  /*  0.4342944819032518
   */
  auto log10e(intval scale) noexcept -> intval;

  /*  3.141592653589793
   */
  auto pi(intval scale) noexcept -> intval;

  /*  0.3183098861837907
   */
  auto inv_pi(intval scale) noexcept -> intval;

  /*  0.5641895835477563
   */
  auto invsqrt_pi(intval scale) noexcept -> intval;

  /*  0.6931471805599453
   */
  auto ln2(intval scale) noexcept -> intval;

  /*  2.302585092994046
   */
  auto ln10(intval scale) noexcept -> intval;

  /*  1.4142135623730951
   */
  auto sqrt2(intval scale) noexcept -> intval;

  /*  1.7320508075688772
   */
  auto sqrt3(intval scale) noexcept -> intval;

  /*  0.5773502691896257
   */
  auto inv_sqrt3(intval scale) noexcept -> intval;

  /*  0.5772156649015329
   */
  auto egamma(intval scale) noexcept -> intval;

  /*  1.618033988749895
   */
  auto phi(intval scale) noexcept -> intval;

  auto sqrt(intval x) noexcept -> intval;

  auto sqrt(intval x, intval scale) noexcept -> intval;

  auto pow(intval x, intval y, intval scale) noexcept -> intval;

  auto exp(intval x, intval scale) noexcept -> intval;

  auto exp2(intval x, intval scale) noexcept -> intval;

  auto log(intval x, intval scale) noexcept -> intval;

  auto log2(intval x, intval scale) noexcept -> intval;

  auto log10(intval x, intval scale) noexcept -> intval;

  auto sin_2pi(intval x, intval scale) noexcept -> intval;

  auto sin(intval x, intval scale) noexcept -> intval;

  auto cos(intval x, intval scale) noexcept -> intval;

  auto tan(intval x, intval scale) noexcept -> intval;

  auto asin(intval x, intval scale) noexcept -> intval;

  auto acos(intval x, intval scale) noexcept -> intval;

  auto atan(intval x, intval scale) noexcept -> intval;

  auto atan2(intval y, intval x, intval scale) noexcept -> intval;
}

#endif
