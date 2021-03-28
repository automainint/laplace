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
  constexpr auto constant_scale() noexcept -> intval;

  constexpr auto div(const intval x, const intval y,
                     const intval scale) noexcept -> intval;

  constexpr auto mul(const intval x, const intval y,
                     const intval scale) noexcept -> intval;

  /*  2.718281828459045
   */
  constexpr auto e(const intval scale) noexcept -> intval;

  /*  1.4426950408889634
   */
  constexpr auto log2e(const intval scale) noexcept -> intval;

  /*  0.4342944819032518
   */
  constexpr auto log10e(const intval scale) noexcept -> intval;

  /*  3.141592653589793
   */
  constexpr auto pi(const intval scale) noexcept -> intval;

  /*  0.3183098861837907
   */
  constexpr auto inv_pi(const intval scale) noexcept -> intval;

  /*  0.5641895835477563
   */
  constexpr auto invsqrt_pi(const intval scale) noexcept -> intval;

  /*  0.6931471805599453
   */
  constexpr auto ln2(const intval scale) noexcept -> intval;

  /*  2.302585092994046
   */
  constexpr auto ln10(const intval scale) noexcept -> intval;

  /*  1.4142135623730951
   */
  constexpr auto sqrt2(const intval scale) noexcept -> intval;

  /*  1.7320508075688772
   */
  constexpr auto sqrt3(const intval scale) noexcept -> intval;

  /*  0.5773502691896257
   */
  constexpr auto inv_sqrt3(const intval scale) noexcept -> intval;

  /*  0.5772156649015329
   */
  constexpr auto egamma(const intval scale) noexcept -> intval;

  /*  1.618033988749895
   */
  constexpr auto phi(const intval scale) noexcept -> intval;

  constexpr auto sqrt(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto pow(const intval x, const intval y,
                     const intval scale) noexcept -> intval;

  constexpr auto exp(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto exp2(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto log(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto log2(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto log10(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto sin_2pi(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto sin(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto cos(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto tan(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto asin(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto acos(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto atan(const intval x, const intval scale) noexcept
      -> intval;

  constexpr auto atan2(const intval y, const intval x,
                       const intval scale) noexcept -> intval;
}

#include "integral.impl.h"

#endif
