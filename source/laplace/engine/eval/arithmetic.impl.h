/*  Copyright (c) 2022 Mitya Selivanov
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

  [[nodiscard]] constexpr auto wrap_add(int8_t x, int8_t y) noexcept
      -> int8_t {
    return static_cast<int8_t>(static_cast<uint8_t>(x) +
                               static_cast<uint8_t>(y));
  }

  [[nodiscard]] constexpr auto wrap_add(int32_t x, int32_t y) noexcept
      -> int32_t {
    return static_cast<int32_t>(static_cast<uint32_t>(x) +
                                static_cast<uint32_t>(y));
  }

  [[nodiscard]] constexpr auto wrap_add(int64_t x, int64_t y) noexcept
      -> int64_t {
    return static_cast<int64_t>(static_cast<uint64_t>(x) +
                                static_cast<uint64_t>(y));
  }

  [[nodiscard]] constexpr auto wrap_sub(int8_t x, int8_t y) noexcept
      -> int8_t {
    return static_cast<int8_t>(static_cast<uint8_t>(x) -
                               static_cast<uint8_t>(y));
  }

  [[nodiscard]] constexpr auto wrap_sub(int32_t x, int32_t y) noexcept
      -> int32_t {
    return static_cast<int32_t>(static_cast<uint32_t>(x) -
                                static_cast<uint32_t>(y));
  }

  [[nodiscard]] constexpr auto wrap_sub(int64_t x, int64_t y) noexcept
      -> int64_t {
    return static_cast<int64_t>(static_cast<uint64_t>(x) -
                                static_cast<uint64_t>(y));
  }

  /*  Safe integer division.
   */
  [[nodiscard]] constexpr auto _div(intval const x,
                                    intval const y) noexcept
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
  [[nodiscard]] constexpr auto _mul(intval const x,
                                    intval const y) noexcept
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
  [[nodiscard]] constexpr auto _add(intval const x,
                                    intval const y) noexcept
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
  [[nodiscard]] constexpr auto _sub(intval const x,
                                    intval const y) noexcept
      -> intval {
    if (y == _int_min) {
      if (x < 0)
        return _int_max + (x + 1);
      return _int_max;
    }
    return _add(x, -y);
  }

  [[nodiscard]] constexpr auto _sub(intval const x) noexcept
      -> intval {
    if (x == _int_min)
      return _int_max;
    return -x;
  }

  /*  Rounding fixed-point number division.
   */
  [[nodiscard]] constexpr auto div(intval const x, intval const y,
                                   intval const scale) noexcept
      -> intval {

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
  [[nodiscard]] constexpr auto mul(intval const x, intval const y,
                                   intval const scale) noexcept
      -> intval {

    return div(_mul(x, y), _mul(scale, scale), scale);
  }

  [[nodiscard]] constexpr auto div_sum(intval const x, intval const y,
                                       intval const divisor) noexcept
      -> intval {
    if ((y < 0 && x <= _int_min - y) || (y > 0 && x >= _int_max - y))
      return _add(_div(x, divisor), _div(y, divisor));
    return _div(x + y, divisor);
  }

  [[nodiscard]] constexpr auto div_sum(intval const x, intval const y,
                                       intval const z,
                                       intval const divisor) noexcept
      -> intval {
    auto const sum_x_y = _add(x, y);
    if ((z < 0 && sum_x_y <= _int_min - z) ||
        (z > 0 && sum_x_y >= _int_max - z))
      return _add(_add(_div(x, divisor), _div(y, divisor)),
                  _div(z, divisor));
    return _div(_add(sum_x_y, z), divisor);
  }
}

#endif
