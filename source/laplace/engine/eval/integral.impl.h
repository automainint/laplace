/*  laplace/engine/eval/integral.impl.h
 *
 *      Integral deterministic math features. Constexpr
 *      implementation.
 *
 *      Make sure to set compiler constexpr evaluation
 *      options high enough.
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

#ifndef laplace_engine_eval_integral_impl_h
#define laplace_engine_eval_integral_impl_h

namespace laplace::engine::eval::impl {
  constexpr intval _constant_scale = 0x10000000;

  constexpr sl::whole _sin_table_size  = 0x200;
  constexpr intval    _sin_table_scale = 0x100;
  constexpr intval    _sin_iterations  = 10;

  constexpr sl::whole _exp_table_size  = 0x200;
  constexpr intval    _exp_table_scale = 0x100;
  constexpr intval    _exp_table_max   = 8;
  constexpr intval    _exp_iterations  = 10;

  constexpr sl::whole _log_table_size  = 0x800;
  constexpr intval    _log_table_scale = 0x400;
  constexpr intval    _log_table_max   = 32;
  constexpr intval    _log_iterations  = 55;

  constexpr sl::whole _sqrt_table_size  = 0x800;
  constexpr intval    _sqrt_table_scale = 0x200;
  constexpr intval    _sqrt_table_max   = 32;

  constexpr auto constant_scale() noexcept -> intval {
    return _constant_scale;
  }

  constexpr auto _int_min    = std::numeric_limits<intval>::min();
  constexpr auto _int_max    = std::numeric_limits<intval>::max();
  constexpr auto _mul_safety = sizeof(intval) >= 8
                                   ? intval { 0xb504f333ll }
                                   : intval { 0xb504 };

  consteval auto _find_pow(intval const x,
                           intval const y,
                           intval const x_scale) noexcept {
    auto z = x;
    for (auto k = 1; k < y; k++) z = (z * x + x_scale / 2) / x_scale;
    return z;
  }

  consteval auto _find_sqrt(intval const x) noexcept {
    auto s = std::min<intval>(x / 2, _mul_safety);
    auto y = s;

    while (s > 0) {
      if (y >= _mul_safety)
        y -= s;
      else if (y * y == x)
        return y;
      else if (y * y < x)
        y += s;
      else
        y -= s;
      s /= 2;
    }

    while (y < _mul_safety && y * y < x) y++;
    while (y < _mul_safety && y * y > x) y--;

    if (y < _mul_safety && y * y < x) {
      auto const x0 = y * y;
      auto const x1 = (y + 1) * (y + 1);
      if (x1 - x < x - x0)
        return y + 1;
    } else if (y < _mul_safety && y * y > x) {
      auto const x0 = (y - 1) * (y - 1);
      auto const x1 = y * y;
      if (x - x0 < x1 - x)
        return y - 1;
    }

    return y;
  }

  /*  Safe integer division.
   */
  constexpr auto _div(const intval x, const intval y) noexcept
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
  constexpr auto _mul(const intval x, const intval y) noexcept
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
  constexpr auto _add(const intval x, const intval y) noexcept
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
  constexpr auto _sub(const intval x, const intval y) noexcept
      -> intval {
    return _add(x, -y);
  }

  /*  Rounding fixed-point number division.
   */
  constexpr auto div(const intval x,
                     const intval y,
                     const intval scale) noexcept -> intval {

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

    const auto half_y = y / 2;

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
  constexpr auto mul(const intval x,
                     const intval y,
                     const intval scale) noexcept -> intval {

    return div(_mul(x, y), _mul(scale, scale), scale);
  }

  constexpr auto e(const intval scale) noexcept -> intval {
    constexpr auto value = div(27182818285, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto log2e(const intval scale) noexcept -> intval {
    constexpr auto value = div(14426950408, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto log10e(const intval scale) noexcept -> intval {
    constexpr auto value = div(4342944819, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(31415926535, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto inv_pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(3183098862, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto invsqrt_pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(5641895835, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto ln2(const intval scale) noexcept -> intval {
    constexpr auto value = div(6931471806, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto ln10(const intval scale) noexcept -> intval {
    constexpr auto value = div(23025850930, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto sqrt2(const intval scale) noexcept -> intval {
    constexpr auto value = div(14142135624, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto sqrt3(const intval scale) noexcept -> intval {
    constexpr auto value = div(17320508076, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto inv_sqrt3(const intval scale) noexcept -> intval {
    constexpr auto value = div(5773502692, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto egamma(const intval scale) noexcept -> intval {
    constexpr auto value = div(5772156649, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  constexpr auto phi(const intval scale) noexcept -> intval {
    constexpr auto value = div(16180339887, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  consteval auto _generate_sin()
      -> std::array<intval, _sin_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      auto value = x;
      auto s     = intval { 1 };
      auto t     = x;

      const auto xx = mul(x, x, scale);

      for (intval n = 3; n < limit * 2; n += 2) {
        t = mul(t, xx, scale);
        t = (t + (n - 1) / 2) / (n - 1);
        t = (t + n / 2) / n;
        s = -s;

        value += s * t;
      }

      return value;
    };

    auto v = std::array<intval, _sin_table_size> {};

    for (sl::index i = 0; i < v.size(); i++) {
      const auto x = div(static_cast<intval>(i), _sin_table_size,
                         2 * pi(_sin_table_scale));

      v[i] = approx(x, _sin_table_scale, _sin_iterations);
    }

    return v;
  }

  consteval auto _generate_exp()
      -> std::array<intval, _exp_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      auto value = scale + x;
      auto t     = x;

      for (intval n = 2; n < limit; n++) {
        t = mul(t, x, scale);
        t = (t + n / 2) / n;

        value += t;
      }

      return value;
    };

    auto v = std::array<intval, _exp_table_size> {};

    for (sl::index i = 0; i < v.size(); i++) {
      const auto x = div(static_cast<intval>(i) * _exp_table_max,
                         _exp_table_size, _exp_table_scale);

      v[i] = approx(x, _exp_table_scale, _exp_iterations);
    }

    return v;
  }

  consteval auto _generate_log()
      -> std::array<intval, _log_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      const auto z0 = x - scale;
      const auto z1 = x + scale;

      auto t     = div(z0, z1, scale);
      auto value = t;

      for (intval n = 3; n <= limit; n += 2) {
        t = mul(t, z0, scale);
        t = div(t, z1, scale);
        t = mul(t, z0, scale);
        t = div(t, z1, scale);

        value += div(t, n, 1);
      }

      return 2 * value;
    };

    auto v = std::array<intval, _log_table_size> {};

    for (sl::index i = 1; i <= v.size(); i++) {
      const auto x = div(static_cast<intval>(i) * _log_table_max,
                         _log_table_size, _log_table_scale);

      v[i - 1] = approx(x, _log_table_scale, _log_iterations);
    }

    return v;
  }

  consteval auto _generate_sqrt()
      -> std::array<intval, _sqrt_table_size> {

    auto v = std::array<intval, _sqrt_table_size> {};

    constexpr auto f = _sqrt_table_scale * _sqrt_table_scale *
                       _sqrt_table_max;

    for (sl::index i = 1; i <= v.size(); i++) {
      const auto x = div(f * static_cast<intval>(i), _sqrt_table_size,
                         1);

      v[i - 1] = _find_sqrt(x);
    }

    return v;
  }

  static constexpr auto _sin  = _generate_sin();
  static constexpr auto _exp  = _generate_exp();
  static constexpr auto _log  = _generate_log();
  static constexpr auto _sqrt = _generate_sqrt();

  template <typename func_>
  constexpr auto _interpolate(const func_  f,
                              const intval f_scale,
                              const intval x,
                              const intval x_scale) noexcept
      -> intval {

    if (x_scale <= f_scale)
      return f(div(x, x_scale, f_scale));

    const auto halfstep = (x_scale + f_scale) / (2 * f_scale);

    const auto f0 = div(x - halfstep, x_scale, f_scale);
    const auto f1 = f0 + 1;

    const auto x0 = div(f0, f_scale, x_scale);
    const auto x1 = div(f1, f_scale, x_scale);

    const auto k     = x - x0;
    const auto delta = x1 - x0;

    const auto y0 = f(f0);
    const auto y1 = f(f0 + 1);

    return y0 + div((y1 - y0) * k, delta, 1);
  }

  constexpr auto sin_table(const intval index) noexcept -> intval {
    if (index < 0)
      return 0;

    if (index >= _sin.size())
      return _sin.back();

    return _sin[index];
  }

  constexpr auto exp_table(const intval index) noexcept -> intval {
    if (index < 0)
      return 0;

    if (index >= _exp.size())
      return _exp.back();

    return _exp[index];
  }

  constexpr auto log_table(const intval index) noexcept -> intval {
    if (index <= 0)
      return std::numeric_limits<intval>::min();

    if (index > _log.size())
      return _log.back();

    return _log[index - 1];
  }

  constexpr auto sqrt_table(const intval index) noexcept -> intval {
    if (index <= 0)
      return 0;

    if (index >= _sqrt.size())
      return _sqrt.back();

    return _sqrt[index - 1];
  }

  constexpr auto exp(const intval x, const intval scale) noexcept
      -> intval {

    if (scale == 0)
      return 0;

    if (x < 0)
      return div(scale, exp(-x, scale), scale);

    const auto delta = _exp_table_max * scale;

    auto value = _interpolate(exp_table, _exp_table_size, x % delta,
                              delta);

    constexpr auto g = _find_pow(e(_exp_table_size), _exp_table_max,
                                 _exp_table_size);

    for (auto z = x - delta; z >= 0; z -= delta)
      value = mul(value, g, _exp_table_size);

    return div(value, _exp_table_scale, scale);
  }

  constexpr auto log(const intval x, const intval scale) noexcept
      -> intval {

    if (scale == 0)
      return 0;

    auto const delta = _log_table_max * scale;
    auto const inc   = _log.back();

    auto value = intval {};
    auto z     = x;

    /*  log(x * y) = log(x) + log(y)
     */

    while (z > delta) {
      value = _add(value, inc);
      z     = div(z, delta, scale);
    }

    if (z > 0) {
      value = _add(
          value, _interpolate(log_table, _log_table_size, z, delta));
    }

    return div(value, _log_table_scale, scale);
  }

  constexpr auto sqrt(const intval x, const intval scale) noexcept
      -> intval {

    if (x <= 0 || scale == 0)
      return 0;

    const auto f = [](const intval x, const intval scale) -> intval {
      const auto delta = _sqrt_table_max * scale;

      auto value = scale;
      auto z     = x;

      constexpr auto g = _find_sqrt(
          _sqrt_table_max * _sqrt_table_scale * _sqrt_table_scale);

      /*  sqrt(x) = sqrt(x / m) * sqrt(m)
       */

      while (z > delta) {
        value = mul(value, div(g, _sqrt_table_scale, scale), scale);
        z     = div(z, delta, scale);
      }

      const auto y = _interpolate(sqrt_table, _sqrt_table_size, z,
                                  delta);

      return mul(value, div(y, _sqrt_table_scale, scale), scale);
    };

    constexpr intval precision = 32;

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto pow(const intval x,
                     const intval y,
                     const intval scale) noexcept -> intval {

    const auto f = [](const intval x, const intval y,
                      const intval scale) {
      return exp(mul(log(x, scale), y, scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, y * precision, scale * precision),
               precision, 1);
  }

  constexpr auto exp2(const intval x, const intval scale) noexcept
      -> intval {

    return pow(scale * 2, x, scale);
  }

  constexpr auto log2(const intval x, const intval scale) noexcept
      -> intval {

    constexpr intval precision = 8;

    const auto f = [](const intval x, const intval scale) {
      return div(log(x, scale), ln2(scale), scale);
    };

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto log10(const intval x, const intval scale) noexcept
      -> intval {

    const auto f = [](const intval x, const intval scale) {
      return div(log(x, scale), ln10(scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto sin_2pi(const intval x, const intval scale) noexcept
      -> intval {

    if (scale == 0) {
      return 0;
    }

    if (x < 0) {
      return -sin_2pi(x, scale);
    }

    if (x > scale) {
      return sin_2pi(x % scale, scale);
    }

    const auto value = _interpolate(sin_table, _sin_table_size, x,
                                    scale);

    return div(value, _sin_table_scale, scale);
  }

  constexpr auto sin(const intval x, const intval scale) noexcept
      -> intval {

    const auto f = [](const intval x, const intval scale) {
      return div(sin_2pi(x, 2 * pi(scale)), 2 * pi(scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto cos(const intval x, const intval scale) noexcept
      -> intval {

    const auto f = [](const intval x, const intval scale) {
      return div(sin_2pi(x + pi(scale) / 2, 2 * pi(scale)),
                 2 * pi(scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto tan(const intval x, const intval scale) noexcept
      -> intval {

    const auto f = [](const intval x, const intval scale) {
      return div(sin(x, scale), cos(x, scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, scale * precision), precision, 1);
  }

  constexpr auto asin(const intval x, const intval scale) noexcept
      -> intval {
    /*  TODO
     */
    return 0;
  }

  constexpr auto acos(const intval x, const intval scale) noexcept
      -> intval {

    return pi(scale) / 2 - asin(x, scale);
  }

  constexpr auto atan(const intval x, const intval scale) noexcept
      -> intval {
    /*  TODO
     */
    return 0;
  }

  constexpr auto atan2(const intval y,
                       const intval x,
                       const intval scale) noexcept -> intval {

    const auto f = [](const intval y, const intval x,
                      const intval scale) -> intval {
      if (x > 0) {
        return atan(div(y, x, scale), scale);
      }

      if (x < 0) {
        if (y >= 0) {
          return atan(div(y, x, scale), scale) + pi(scale);
        }

        return atan(div(y, x, scale), scale) - pi(scale);
      }

      if (y > 0) {
        return pi(scale) / 2;
      }

      if (y < 0) {
        return -pi(scale) / 2;
      }

      return 0;
    };

    constexpr intval precision = 8;

    return div(f(y * precision, x * precision, scale * precision),
               precision, 1);
  }
}

#endif
