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
  auto pi(vecval scale) -> vecval;
  auto e(vecval scale) -> vecval;

  auto sqrt(vecval x) -> vecval;
  auto sqrt(vecval x, vecval scale) -> vecval;

  auto pow(vecval x, vecval y) -> vecval;
  auto pow(vecval x, vecval y, vecval scale) -> vecval;

  auto exp(vecval x, vecval scale) -> vecval;
  auto exp2(vecval x, vecval scale) -> vecval;
  auto log(vecval x, vecval scale) -> vecval;
  auto log2(vecval x, vecval scale) -> vecval;
  auto log10(vecval x, vecval scale) -> vecval;

  auto sin(vecval x, vecval scale) -> vecval;
  auto cos(vecval x, vecval scale) -> vecval;
  auto tan(vecval x, vecval scale) -> vecval;

  auto asin(vecval x, vecval scale) -> vecval;
  auto acos(vecval x, vecval scale) -> vecval;
  auto atan(vecval x, vecval scale) -> vecval;
  auto atan2(vecval y, vecval x, vecval scale) -> vecval;
}

#endif
