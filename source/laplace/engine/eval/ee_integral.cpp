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
  /*  Tests for 64-bit integer.
   */

  static_assert(_find_sqrt(4) == 2);
  static_assert(_find_sqrt(5) == 2);
  static_assert(_find_sqrt(6) == 2);
  static_assert(_find_sqrt(7) == 3);
  static_assert(_find_sqrt(8) == 3);
  static_assert(_find_sqrt(9) == 3);
  static_assert(_find_sqrt(99) == 10);
  static_assert(_find_sqrt(100) == 10);
  static_assert(_find_sqrt(101) == 10);
  static_assert(_find_sqrt(0xffffffffffff) == 0x1000000);
  static_assert(_find_sqrt(0x1000000000000) == 0x1000000);
  static_assert(_find_sqrt(0x1000000000001) == 0x1000000);

  static_assert(_mul(0x10000, 0x10000) == 0x100000000);
  static_assert(_mul(-0x10000, 0x10000) == -0x100000000);
  static_assert(_mul(0x10000, -0x10000) == -0x100000000);
  static_assert(_mul(-0x10000, -0x10000) == 0x100000000);
  static_assert(_mul(0x100000000, 0x100) == 0x10000000000);
  static_assert(_mul(-0x100000000, 0x100) == -0x10000000000);
  static_assert(_mul(0x100000000, -0x100) == -0x10000000000);
  static_assert(_mul(-0x100000000, -0x100) == 0x10000000000);
  static_assert(_mul(0x1000000000000000, 0x1000000000000000) ==
                _int_max);
  static_assert(_mul(-0x1000000000000000, 0x1000000000000000) ==
                _int_min);
  static_assert(_mul(0x1000000000000000, -0x1000000000000000) ==
                _int_min);
  static_assert(_mul(-0x1000000000000000, -0x1000000000000000) ==
                _int_max);

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
  static_assert(div(1, 0, 1) == _int_max);
  static_assert(div(-1, 0, 1) == _int_min);
  static_assert(div(0, 0, 1) == 1);
  static_assert(div(0, 0, 100) == 100);
  static_assert(div(0x10000000000000, 1, 0x10000000000000) ==
                _int_max);
  static_assert(div(0x10000000000000,
                    0x10000000000000,
                    0x10000000000000) == 0x10000000000000);

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
  static_assert(log10(10000, 100) == 200);
  static_assert(exp(log(200, 100), 100) < 202);
  static_assert(exp(log(200, 100), 100) > 198);
  static_assert(pow(200, 200, 100) < 402);
  static_assert(pow(200, 200, 100) > 398);
  static_assert(sqrt(400, 100) == 200);
  static_assert(sqrt(9, 1) == 3);
  static_assert(sqrt(81, 1) == 9);
  static_assert(sqrt(10000, 1) == 100);

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

  auto add(intval const x, intval const y) noexcept -> intval {
    return impl::_add(x, y);
  }

  auto op_add(intval x, intval y) noexcept -> intval {
    return impl::_add(x, y);
  }

  auto sub(intval const x, intval const y) noexcept -> intval {
    return impl::_sub(x, y);
  }

  auto op_sub(intval x, intval y) noexcept -> intval {
    return impl::_sub(x, y);
  }

  auto div(intval const x, intval const y) noexcept -> intval {
    return impl::div(x, y, 1);
  }

  auto op_div(intval x, intval y) noexcept -> intval {
    return impl::div(x, y, 1);
  }

  auto div(intval const x,
           intval const y,
           intval const scale) noexcept -> intval {
    return impl::div(x, y, scale);
  }

  auto mul(intval const x, intval const y) noexcept -> intval {
    return impl::_mul(x, y);
  }

  auto op_mul(intval x, intval y) noexcept -> intval {
    return impl::_mul(x, y);
  }

  auto mul(intval const x,
           intval const y,
           intval const scale) noexcept -> intval {
    return impl::mul(x, y, scale);
  }

  auto sqr(intval x) noexcept -> intval {
    return impl::_mul(x, x);
  }

  auto sqr(intval x, intval scale) noexcept -> intval {
    return impl::mul(x, x, scale);
  }

  auto div_sum(intval x, intval y, intval divisor) noexcept
      -> intval {
    return impl::div_sum(x, y, divisor);
  }

  auto div_sum(intval x, intval y, intval z, intval divisor) noexcept
      -> intval {
    return impl::div_sum(x, y, z, divisor);
  }

  auto e(intval const scale) noexcept -> intval {
    return impl::e(scale);
  }

  auto log2e(intval const scale) noexcept -> intval {
    return impl::log2e(scale);
  }

  auto log10e(intval const scale) noexcept -> intval {
    return impl::log10e(scale);
  }

  auto pi(intval const scale) noexcept -> intval {
    return impl::pi(scale);
  }

  auto inv_pi(intval const scale) noexcept -> intval {
    return impl::inv_pi(scale);
  }

  auto invsqrt_pi(intval const scale) noexcept -> intval {
    return impl::invsqrt_pi(scale);
  }

  auto ln2(intval const scale) noexcept -> intval {
    return impl::ln2(scale);
  }

  auto ln10(intval const scale) noexcept -> intval {
    return impl::ln10(scale);
  }

  auto sqrt2(intval const scale) noexcept -> intval {
    return impl::sqrt2(scale);
  }

  auto sqrt3(intval const scale) noexcept -> intval {
    return impl::sqrt3(scale);
  }

  auto inv_sqrt3(intval const scale) noexcept -> intval {
    return impl::inv_sqrt3(scale);
  }

  auto egamma(intval const scale) noexcept -> intval {
    return impl::egamma(scale);
  }

  auto phi(intval const scale) noexcept -> intval {
    return impl::phi(scale);
  }

  auto sqrt(intval const x, intval const scale) noexcept -> intval {
    return impl::sqrt(x, scale);
  }

  auto pow(intval const x,
           intval const y,
           intval const scale) noexcept -> intval {
    return impl::pow(x, y, scale);
  }

  auto exp(intval const x, intval const scale) noexcept -> intval {
    return impl::exp(x, scale);
  }

  auto exp2(intval const x, intval const scale) noexcept -> intval {
    return impl::exp2(x, scale);
  }

  auto log(intval const x, intval const scale) noexcept -> intval {
    return impl::log(x, scale);
  }

  auto log2(intval const x, intval const scale) noexcept -> intval {
    return impl::log2(x, scale);
  }

  auto log10(intval const x, intval const scale) noexcept -> intval {
    return impl::log10(x, scale);
  }

  auto sin_2pi(intval const x, intval const scale) noexcept
      -> intval {
    return impl::sin_2pi(x, scale);
  }

  auto sin(intval const x, intval const scale) noexcept -> intval {
    return impl::sin(x, scale);
  }

  auto cos(intval const x, intval const scale) noexcept -> intval {
    return impl::cos(x, scale);
  }

  auto tan(intval const x, intval const scale) noexcept -> intval {
    return impl::tan(x, scale);
  }

  auto asin(intval const x, intval const scale) noexcept -> intval {
    return impl::asin(x, scale);
  }

  auto acos(intval const x, intval const scale) noexcept -> intval {
    return impl::acos(x, scale);
  }

  auto atan(intval const x, intval const scale) noexcept -> intval {
    return impl::atan(x, scale);
  }

  auto atan2(intval const y,
             intval const x,
             intval const scale) noexcept -> intval {
    return impl::atan2(y, x, scale);
  }
}
