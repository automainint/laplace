/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_eval_arithmetic_impl_h
#define laplace_engine_eval_arithmetic_impl_h

#include "../defs.h"

namespace laplace::engine::eval::impl {
  constexpr auto _int_min    = std::numeric_limits<intval>::min();
  constexpr auto _int_max    = std::numeric_limits<intval>::max();
  constexpr auto _mul_safety = sizeof(intval) >= 8
                                   ? intval { 0xb504f333ll }
                                   : intval { 0xb504 };

  /*  Safe integer division.
   */
  constexpr auto _div(intval const x, intval const y) noexcept
      -> intval {
    if (y != 0)
      return x / y;
    if (x < 0)
      return _int_min;
    if (x > 0)
      return _int_max;
    return 1;
  }

  /*  Safe integer multiplication.
   */
  constexpr auto _mul(intval const x, intval const y) noexcept
      -> intval {
    if (x >= _mul_safety) {
      if (y > _int_max / x)
        return _int_max;
      if (y < _int_min / x)
        return _int_min;
    }
    if (x <= -_mul_safety) {
      if (y > _int_min / x)
        return _int_min;
      if (y < _int_max / x)
        return _int_max;
    }
    if (y >= _mul_safety) {
      if (x > _int_max / y)
        return _int_max;
      if (x < _int_min / y)
        return _int_min;
    }
    if (y <= -_mul_safety) {
      if (x > _int_min / y)
        return _int_min;
      if (x < _int_max / y)
        return _int_max;
    }
    return x * y;
  }

  /*  Safe integer addition.
   */
  constexpr auto _add(intval const x, intval const y) noexcept
      -> intval {
    if (x > 0 && y < 0)
      return x + y;
    if (x < 0 && y > 0)
      return x + y;
    if (y < 0 && x <= _int_min - y)
      return _int_min;
    if (y > 0 && x >= _int_max - y)
      return _int_max;
    return x + y;
  }

  /*  Safe integer subtraction.
   */
  constexpr auto _sub(intval const x, intval const y) noexcept
      -> intval {
    if (y == _int_min) {
      if (x < 0)
        return _int_max + x;
      return _int_max;
    }
    return _add(x, -y);
  }

  constexpr auto _sub(intval const x) noexcept -> intval {
    if (x == _int_min)
      return _int_max;
    return -x;
  }

  /*  Rounding fixed-point number division.
   */
  constexpr auto div(intval const x, intval const y,
                     intval const scale) noexcept -> intval {

    if (y == 0) {
      if (x > 0)
        return _int_max;
      if (x < 0)
        return _int_min;
      return scale;
    }

    if (y < 0)
      return -div(x, -y, scale);
    if (scale < 0)
      return -div(x, y, -scale);

    auto const half_y = y / 2;

    if (x < 0) {
      if (x < _int_min / scale)
        return _mul(_div(_sub(x, half_y), y), scale);
      return _div(_sub(_mul(x, scale), half_y), y);
    }

    if (x > _int_max / scale)
      return _mul(_div(_add(x, half_y), y), scale);
    return _div(_add(_mul(x, scale), half_y), y);
  }

  /*  Fixed-point number multiplication.
   */
  constexpr auto mul(intval const x, intval const y,
                     intval const scale) noexcept -> intval {

    return div(_mul(x, y), _mul(scale, scale), scale);
  }

  constexpr auto div_sum(intval const x, intval const y,
                         intval const divisor) noexcept -> intval {
    if ((y < 0 && x <= _int_min - y) || (y > 0 && x >= _int_max - y))
      return _add(_div(x, divisor), _div(y, divisor));
    return _div(x + y, divisor);
  }

  constexpr auto div_sum(intval const x, intval const y,
                         intval const z,
                         intval const divisor) noexcept -> intval {
    auto const sum_x_y = _add(x, y);
    if ((z < 0 && sum_x_y <= _int_min - z) ||
        (z > 0 && sum_x_y >= _int_max - z))
      return _add(_add(_div(x, divisor), _div(y, divisor)),
                  _div(z, divisor));
    return _div(_add(sum_x_y, z), divisor);
  }
}

#endif
