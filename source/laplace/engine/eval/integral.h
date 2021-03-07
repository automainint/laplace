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

#ifndef laplace_engine_eval_math_h
#define laplace_engine_eval_math_h

#include "../defs.h"

namespace laplace::engine::eval {
  auto pi(intval scale) -> intval;
  auto e(intval scale) -> intval;

  auto sqrt(intval x) -> intval;
  auto sqrt(intval x, intval scale) -> intval;

  auto pow(intval x, intval y) -> intval;
  auto pow(intval x, intval y, intval scale) -> intval;

  auto exp(intval x, intval scale) -> intval;
  auto exp2(intval x, intval scale) -> intval;
  auto log(intval x, intval scale) -> intval;
  auto log2(intval x, intval scale) -> intval;
  auto log10(intval x, intval scale) -> intval;

  auto sin(intval x, intval scale) -> intval;
  auto cos(intval x, intval scale) -> intval;
  auto tan(intval x, intval scale) -> intval;

  auto asin(intval x, intval scale) -> intval;
  auto acos(intval x, intval scale) -> intval;
  auto atan(intval x, intval scale) -> intval;
  auto atan2(intval y, intval x, intval scale) -> intval;
}

#endif
