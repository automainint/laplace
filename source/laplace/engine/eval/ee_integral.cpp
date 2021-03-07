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

  inline auto _scale(intval x, intval scale) -> math::realmax_t {

    if (scale == 0) {
      error(__FUNCTION__, "Invalid scale.");
      return 0;
    }

    return static_cast<realmax_t>(x) / scale;
  }

  template <typename func_, typename... args_>
  inline auto _fixed(func_ op, intval scale, args_... args) -> intval {
    return round<intval>(op(_scale(args, scale)...) * scale);
  }

  auto pi(intval scale) -> intval {
    return round<intval>(std::numbers::pi_v<realmax_t> * scale);
  }

  auto e(intval scale) -> intval {
    return round<intval>(std::numbers::e_v<realmax_t> * scale);
  }

  auto sqrt(intval x) -> intval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(sqrtl, 1, x);
  }

  auto sqrt(intval x, intval scale) -> intval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(sqrtl, scale, x);
  }

  auto pow(intval x, intval y) -> intval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(powl, 1, x, y);
  }

  auto pow(intval x, intval y, intval scale) -> intval {
    if (x < 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(powl, scale, x, y);
  }

  auto exp(intval x, intval scale) -> intval {
    return _fixed(expl, scale, x);
  }

  auto exp2(intval x, intval scale) -> intval {
    return _fixed(exp2l, scale, x);
  }

  auto log(intval x, intval scale) -> intval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(logl, scale, x);
  }

  auto log2(intval x, intval scale) -> intval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(log2l, scale, x);
  }

  auto log10(intval x, intval scale) -> intval {
    if (x <= 0) {
      error(__FUNCTION__, "Invalid argument.");
      return 0;
    }

    return _fixed(log10l, scale, x);
  }

  auto sin(intval x, intval scale) -> intval {
    return _fixed(sinl, scale, x);
  }

  auto cos(intval x, intval scale) -> intval {
    return _fixed(cosl, scale, x);
  }

  auto tan(intval x, intval scale) -> intval {
    return _fixed(tanl, scale, x);
  }

  auto asin(intval x, intval scale) -> intval {
    return _fixed(asinl, scale, x);
  }

  auto acos(intval x, intval scale) -> intval {
    return _fixed(acosl, scale, x);
  }

  auto atan(intval x, intval scale) -> intval {
    return _fixed(atanl, scale, x);
  }

  auto atan2(intval y, intval x, intval scale) -> intval {
    return _fixed(atan2l, scale, y, x);
  }
}