/*  laplace/engine/eval/ee_integral.cpp
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

#include "../../core/defs.h"
#include "../../math/basic.h"
#include "integral.h"
#include <numbers>

namespace laplace::engine::eval {
  using std::sqrtl, std::sinl, std::cosl, std::tanl, std::asinl,
      std::acosl, std::atanl, std::atan2l, std::powl, std::expl,
      std::exp2l, std::logl, std::log2l, std::log10l,
      math::round, math::realmax_t;

  inline auto _scale(vecval x, vecval scale) -> math::realmax_t {

    if (scale == 0) {
      error(__FUNCTION__, "Invalid scale.");
      return 0;
    }

    return static_cast<realmax_t>(x) / scale;
  }

  template <typename func_, typename... args_>
  inline auto _fixed(func_ op, vecval scale, args_... args) -> vecval {
    return round<vecval>(op(_scale(args, scale)...) * scale);
  }

  auto pi(vecval scale) -> vecval {
    return round<vecval>(std::numbers::pi_v<realmax_t> * scale);
  }

  auto e(vecval scale) -> vecval {
    return round<vecval>(std::numbers::e_v<realmax_t> * scale);
  }

  auto sqrt(vecval x) -> vecval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(sqrtl, 1, x);
  }

  auto sqrt(vecval x, vecval scale) -> vecval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(sqrtl, scale, x);
  }

  auto pow(vecval x, vecval y) -> vecval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(powl, 1, x, y);
  }

  auto pow(vecval x, vecval y, vecval scale) -> vecval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(powl, scale, x, y);
  }

  auto exp(vecval x, vecval scale) -> vecval {
    return _fixed(expl, scale, x);
  }

  auto exp2(vecval x, vecval scale) -> vecval {
    return _fixed(exp2l, scale, x);
  }

  auto log(vecval x, vecval scale) -> vecval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(logl, scale, x);
  }

  auto log2(vecval x, vecval scale) -> vecval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(log2l, scale, x);
  }

  auto log10(vecval x, vecval scale) -> vecval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(log10l, scale, x);
  }

  auto sin(vecval x, vecval scale) -> vecval {
    return _fixed(sinl, scale, x);
  }

  auto cos(vecval x, vecval scale) -> vecval {
    return _fixed(cosl, scale, x);
  }

  auto tan(vecval x, vecval scale) -> vecval {
    return _fixed(tanl, scale, x);
  }

  auto asin(vecval x, vecval scale) -> vecval {
    return _fixed(asinl, scale, x);
  }

  auto acos(vecval x, vecval scale) -> vecval {
    return _fixed(acosl, scale, x);
  }

  auto atan(vecval x, vecval scale) -> vecval {
    return _fixed(atanl, scale, x);
  }

  auto atan2(vecval y, vecval x, vecval scale) -> vecval {
    return _fixed(atan2l, scale, y, x);
  }
}