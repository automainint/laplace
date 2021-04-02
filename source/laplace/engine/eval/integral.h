/*  laplace/engine/eval/integral.h
 *
 *      Integral deterministic math features.
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

#ifndef laplace_engine_eval_integral_h
#define laplace_engine_eval_integral_h

#include "../defs.h"

namespace laplace::engine::eval {
  auto constant_scale() noexcept -> intval;

  auto div(const intval x, const intval y, const intval scale) noexcept
      -> intval;

  auto mul(const intval x, const intval y, const intval scale) noexcept
      -> intval;

  /*  2.718281828459045
   */
  auto e(const intval scale) noexcept -> intval;

  /*  1.4426950408889634
   */
  auto log2e(const intval scale) noexcept -> intval;

  /*  0.4342944819032518
   */
  auto log10e(const intval scale) noexcept -> intval;

  /*  3.141592653589793
   */
  auto pi(const intval scale) noexcept -> intval;

  /*  0.3183098861837907
   */
  auto inv_pi(const intval scale) noexcept -> intval;

  /*  0.5641895835477563
   */
  auto invsqrt_pi(const intval scale) noexcept -> intval;

  /*  0.6931471805599453
   */
  auto ln2(const intval scale) noexcept -> intval;

  /*  2.302585092994046
   */
  auto ln10(const intval scale) noexcept -> intval;

  /*  1.4142135623730951
   */
  auto sqrt2(const intval scale) noexcept -> intval;

  /*  1.7320508075688772
   */
  auto sqrt3(const intval scale) noexcept -> intval;

  /*  0.5773502691896257
   */
  auto inv_sqrt3(const intval scale) noexcept -> intval;

  /*  0.5772156649015329
   */
  auto egamma(const intval scale) noexcept -> intval;

  /*  1.618033988749895
   */
  auto phi(const intval scale) noexcept -> intval;

  auto sqrt(const intval x, const intval scale) noexcept -> intval;

  auto pow(const intval x, const intval y, const intval scale) noexcept
      -> intval;

  auto exp(const intval x, const intval scale) noexcept -> intval;

  auto exp2(const intval x, const intval scale) noexcept -> intval;

  auto log(const intval x, const intval scale) noexcept -> intval;

  auto log2(const intval x, const intval scale) noexcept -> intval;

  auto log10(const intval x, const intval scale) noexcept -> intval;

  auto sin_2pi(const intval x, const intval scale) noexcept -> intval;

  auto sin(const intval x, const intval scale) noexcept -> intval;

  auto cos(const intval x, const intval scale) noexcept -> intval;

  auto tan(const intval x, const intval scale) noexcept -> intval;

  auto asin(const intval x, const intval scale) noexcept -> intval;

  auto acos(const intval x, const intval scale) noexcept -> intval;

  auto atan(const intval x, const intval scale) noexcept -> intval;

  auto atan2(const intval y, const intval x,
             const intval scale) noexcept -> intval;
}

#endif
