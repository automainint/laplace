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

#include "integral.h"

#include "integral.impl.h"

namespace laplace::engine::eval::impl {
  static_assert(div(15, 10, 100) == 150);
  static_assert(div(150, 100, 10) == 15);
  static_assert(div(14, 10, 1) == 1);
  static_assert(div(15, 10, 1) == 2);
  static_assert(div(24, 10, 1) == 2);
  static_assert(div(-14, 10, 1) == -1);
  static_assert(div(-15, 10, 1) == -2);
  static_assert(div(-24, 10, 1) == -2);
  static_assert(div(0x100000000000, 0x100000000000, 1000) == 1000);
  static_assert(div(100000000, 10000000000000000, 100000000) == 1);
  static_assert(div(100000000, 100000000, 10000000000000000) ==
                10000000000000000);

  static_assert(e(1000000) == 2718282);
  static_assert(log2e(1000000) == 1442695);
  static_assert(log10e(1000000) == 434294);
  static_assert(pi(1000000) == 3141593);
  static_assert(inv_pi(1000000) == 318310);
  static_assert(invsqrt_pi(1000000) == 564190);
  static_assert(ln2(1000000) == 693147);
  static_assert(ln10(1000000) == 2302585);
  static_assert(sqrt2(1000000) == 1414214);
  static_assert(sqrt3(1000000) == 1732051);
  static_assert(inv_sqrt3(1000000) == 577350);
  static_assert(egamma(1000000) == 577216);
  static_assert(phi(1000000) == 1618034);

  static_assert(exp(0, 100) == 100);
  static_assert(exp(100, 100) == e(100));
  static_assert(log(e(100), 100) == 100);
  static_assert(log((e(100) * e(100)) / 100, 100) == 200);
  static_assert(exp(log(200, 100), 100) < 202);
  static_assert(exp(log(200, 100), 100) > 198);
  static_assert(pow(200, 200, 100) < 406, "2 * 2 != 4");
  static_assert(pow(200, 200, 100) > 394, "2 * 2 != 4");
  static_assert(sqrt(400, 100) == 200, "sqrt(4) != 2");

  /*  cos^2(x) + sin^2(x) = 1
   */

  static_assert(mul(sin(10, 100), sin(10, 100), 100) +
                    mul(cos(10, 100), cos(10, 100), 100) <
                102);

  static_assert(mul(sin(10, 100), sin(10, 100), 100) +
                    mul(cos(10, 100), cos(10, 100), 100) >
                98);

  static_assert(mul(sin(20, 100), sin(20, 100), 100) +
                    mul(cos(20, 100), cos(20, 100), 100) <
                102);

  static_assert(mul(sin(20, 100), sin(20, 100), 100) +
                    mul(cos(20, 100), cos(20, 100), 100) >
                98);

  static_assert(mul(sin(90, 100), sin(90, 100), 100) +
                    mul(cos(90, 100), cos(90, 100), 100) <
                102);

  static_assert(mul(sin(90, 100), sin(90, 100), 100) +
                    mul(cos(90, 100), cos(90, 100), 100) >
                98);

  static_assert(mul(sin(130, 100), sin(130, 100), 100) +
                    mul(cos(130, 100), cos(130, 100), 100) <
                102);

  static_assert(mul(sin(130, 100), sin(130, 100), 100) +
                    mul(cos(130, 100), cos(130, 100), 100) >
                98);
}

namespace laplace::engine::eval {
  auto constant_scale() noexcept -> intval {
    return impl::constant_scale();
  }

  auto div(const intval x, const intval y, const intval scale) noexcept
      -> intval {
    return impl::div(x, y, scale);
  }

  auto mul(const intval x, const intval y, const intval scale) noexcept
      -> intval {
    return impl::mul(x, y, scale);
  }

  auto e(const intval scale) noexcept -> intval {
    return impl::e(scale);
  }

  auto log2e(const intval scale) noexcept -> intval {
    return impl::log2e(scale);
  }

  auto log10e(const intval scale) noexcept -> intval {
    return impl::log10e(scale);
  }

  auto pi(const intval scale) noexcept -> intval {
    return impl::pi(scale);
  }

  auto inv_pi(const intval scale) noexcept -> intval {
    return impl::inv_pi(scale);
  }

  auto invsqrt_pi(const intval scale) noexcept -> intval {
    return impl::invsqrt_pi(scale);
  }

  auto ln2(const intval scale) noexcept -> intval {
    return impl::ln2(scale);
  }

  auto ln10(const intval scale) noexcept -> intval {
    return impl::ln10(scale);
  }

  auto sqrt2(const intval scale) noexcept -> intval {
    return impl::sqrt2(scale);
  }

  auto sqrt3(const intval scale) noexcept -> intval {
    return impl::sqrt3(scale);
  }

  auto inv_sqrt3(const intval scale) noexcept -> intval {
    return impl::inv_sqrt3(scale);
  }

  auto egamma(const intval scale) noexcept -> intval {
    return impl::egamma(scale);
  }

  auto phi(const intval scale) noexcept -> intval {
    return impl::phi(scale);
  }

  auto sqrt(const intval x, const intval scale) noexcept -> intval {
    return impl::sqrt(x, scale);
  }

  auto pow(const intval x, const intval y, const intval scale) noexcept
      -> intval {
    return impl::pow(x, y, scale);
  }

  auto exp(const intval x, const intval scale) noexcept -> intval {
    return impl::exp(x, scale);
  }

  auto exp2(const intval x, const intval scale) noexcept -> intval {
    return impl::exp2(x, scale);
  }

  auto log(const intval x, const intval scale) noexcept -> intval {
    return impl::log(x, scale);
  }

  auto log2(const intval x, const intval scale) noexcept -> intval {
    return impl::log2(x, scale);
  }

  auto log10(const intval x, const intval scale) noexcept -> intval {
    return impl::log10(x, scale);
  }

  auto sin_2pi(const intval x, const intval scale) noexcept -> intval {
    return impl::sin_2pi(x, scale);
  }

  auto sin(const intval x, const intval scale) noexcept -> intval {
    return impl::sin(x, scale);
  }

  auto cos(const intval x, const intval scale) noexcept -> intval {
    return impl::cos(x, scale);
  }

  auto tan(const intval x, const intval scale) noexcept -> intval {
    return impl::tan(x, scale);
  }

  auto asin(const intval x, const intval scale) noexcept -> intval {
    return impl::asin(x, scale);
  }

  auto acos(const intval x, const intval scale) noexcept -> intval {
    return impl::acos(x, scale);
  }

  auto atan(const intval x, const intval scale) noexcept -> intval {
    return impl::atan(x, scale);
  }

  auto atan2(const intval y, const intval x,
             const intval scale) noexcept -> intval {
    return impl::atan2(y, x, scale);
  }
}
