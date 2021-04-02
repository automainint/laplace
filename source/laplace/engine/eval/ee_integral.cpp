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
