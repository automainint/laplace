/*  laplace/engine/eval/integral.impl.h
 *
 *      Integral deterministic math features. Constexpr implementation.
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
  constexpr intval    _sin_interations = 10;

  constexpr sl::whole _exp_table_size  = 0x200;
  constexpr intval    _exp_table_scale = 0x100;
  constexpr intval    _exp_interations = 10;

  constexpr sl::whole _log_table_size  = 0x400;
  constexpr intval    _log_table_scale = 0x200;
  constexpr intval    _log_interations = 10;

  constexpr auto constant_scale() noexcept -> intval {
    return _constant_scale;
  }

  constexpr auto div(const intval x, const intval y,
                     const intval scale) noexcept -> intval {

    constexpr auto int_max = std::numeric_limits<intval>::max();
    constexpr auto int_min = -int_max;

    if (y == 0) {
      if (x > 0) {
        return int_max;
      } else if (x < 0) {
        return int_min;
      } else {
        return 1;
      }
    }

    if (y < 0) {
      return -div(x, -y, scale);
    }

    if (scale < 0) {
      return -div(x, y, -scale);
    }

    const auto y_delta = y / 2;

    if (x < 0) {
      if (-x > int_max / scale) {
        const auto d = (x - y_delta) / y;

        if (-d > int_max / scale) {
          return int_min;
        }

        return d * scale;
      }

      return (x * scale - y_delta) / y;
    }

    if (x > int_max / scale) {
      const auto d = (x + y_delta) / y;

      if (d > int_max / scale) {
        return int_max;
      }

      return d * scale;
    }

    return (x * scale + y_delta) / y;
  }

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

  constexpr auto mul(const intval x, const intval y,
                     const intval scale) noexcept -> intval {

    return div(x * y, scale * scale, scale);
  }

  constexpr auto e(const intval scale) noexcept -> intval {
    constexpr auto value = div(27182818285, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(e(1000000) == 2718282);

  constexpr auto log2e(const intval scale) noexcept -> intval {
    constexpr auto value = div(14426950408, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(log2e(1000000) == 1442695);

  constexpr auto log10e(const intval scale) noexcept -> intval {
    constexpr auto value = div(4342944819, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(log10e(1000000) == 434294);

  constexpr auto pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(31415926535, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(pi(1000000) == 3141593);

  constexpr auto inv_pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(3183098862, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(inv_pi(1000000) == 318310);

  constexpr auto invsqrt_pi(const intval scale) noexcept -> intval {
    constexpr auto value = div(5641895835, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(invsqrt_pi(1000000) == 564190);

  constexpr auto ln2(const intval scale) noexcept -> intval {
    constexpr auto value = div(6931471806, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(ln2(1000000) == 693147);

  constexpr auto ln10(const intval scale) noexcept -> intval {
    constexpr auto value = div(23025850930, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(ln10(1000000) == 2302585);

  constexpr auto sqrt2(const intval scale) noexcept -> intval {
    constexpr auto value = div(14142135624, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(sqrt2(1000000) == 1414214);

  constexpr auto sqrt3(const intval scale) noexcept -> intval {
    constexpr auto value = div(17320508076, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(sqrt3(1000000) == 1732051);

  constexpr auto inv_sqrt3(const intval scale) noexcept -> intval {
    constexpr auto value = div(5773502692, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(inv_sqrt3(1000000) == 577350);

  constexpr auto egamma(const intval scale) noexcept -> intval {
    constexpr auto value = div(5772156649, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(egamma(1000000) == 577216);

  constexpr auto phi(const intval scale) noexcept -> intval {
    constexpr auto value = div(16180339887, 10000000000,
                               _constant_scale);
    return div(value, _constant_scale, scale);
  }

  static_assert(phi(1000000) == 1618034);

  constexpr auto _generate_sin()
      -> std::array<intval, _sin_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      intval value = x;
      intval s     = 1;
      intval t     = x;

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
      const intval x = div(static_cast<intval>(i), _sin_table_size,
                           2 * pi(_sin_table_scale));

      v[i] = approx(x, _sin_table_scale, _sin_interations);
    }

    return v;
  }

  constexpr auto _generate_exp()
      -> std::array<intval, _exp_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      intval value = scale + x;
      intval t     = x;

      for (intval n = 2; n < limit; n++) {
        t = mul(t, x, scale);
        t = (t + n / 2) / n;

        value += t;
      }

      return value;
    };

    auto v = std::array<intval, _exp_table_size> {};

    for (sl::index i = 0; i < v.size(); i++) {
      const intval x = div(static_cast<intval>(i) * 2,
                           _exp_table_size, _exp_table_scale);

      v[i] = approx(x, _exp_table_scale, _exp_interations);
    }

    return v;
  }

  constexpr auto _generate_log()
      -> std::array<intval, _log_table_size> {

    constexpr auto approx = [](const intval x, const intval scale,
                               const intval limit) {
      const intval z0 = x - scale;
      const intval z1 = x + scale;

      intval t     = div(z0, z1, scale);
      intval value = t;

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

    for (sl::index i = 0; i < v.size(); i++) {
      const intval x = div(static_cast<intval>(i + 1) * 2,
                           _log_table_size, _log_table_scale);

      v[i] = approx(x, _log_table_scale, _log_interations);
    }

    return v;
  }

  static constexpr auto _sin = _generate_sin();
  static constexpr auto _exp = _generate_exp();
  static constexpr auto _log = _generate_log();

  template <typename func_>
  constexpr auto _interpolate(const func_ f, const intval f_scale,
                              const intval x,
                              const intval x_scale) noexcept -> intval {

    if (x_scale <= f_scale) {
      return f(div(x, x_scale, f_scale));
    }

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
    if (index < 0 || index >= static_cast<intval>(_sin.size())) {
      return 0;
    }

    return _sin[index];
  }

  constexpr auto exp_table(const intval index) noexcept -> intval {
    if (index < 0 || index >= static_cast<intval>(_exp.size())) {
      return 0;
    }

    return _exp[index];
  }

  constexpr auto log_table(const intval index) noexcept -> intval {
    if (index <= 0) {
      return std::numeric_limits<intval>::min();
    }

    if (index > static_cast<intval>(_log.size())) {
      return 0;
    }

    return _log[index - 1];
  }

  constexpr auto exp(const intval x, const intval scale) noexcept
      -> intval {

    if (scale == 0) {
      return 0;
    }

    if (x < 0) {
      return div(scale, exp(-x, scale), scale);
    }

    const auto delta = scale * 2;

    intval value = exp_table(div(x % delta, delta, _exp_table_size));

    const auto e2 = mul(e(_exp_table_size), e(_exp_table_size), _exp_table_size);

    for (auto z = x - delta; z >= 0; z -= delta) {
      value = mul(value, e2, _exp_table_size);
    }

    return div(value, _exp_table_scale, scale);
  }

  static_assert(exp(0, 100) == 100);
  static_assert(exp(100, 100) == e(100));

  constexpr auto log(const intval x, const intval scale) noexcept
      -> intval {

    if (scale == 0) {
      return 0;
    }

    const auto delta = scale * 2;

    intval value = 0;
    intval z     = x;

    while (z > delta) {
      value += ln2(_log_table_scale);
      z = div(z, delta, scale);
    }

    if (z > 0) {
      value += log_table(div(z, delta, _log_table_size));
    }

    return div(value, _log_table_scale, scale);
  }

  static_assert(log(e(100), 100) == 100);
  static_assert(log((e(100) * e(100)) / 100, 100) == 200);

  static_assert(exp(log(200, 100), 100) < 202);
  static_assert(exp(log(200, 100), 100) > 198);

  constexpr auto pow(const intval x, const intval y,
                     const intval scale) noexcept -> intval {

    const auto f = [](const intval x, const intval y,
                      const intval scale) {
      return exp(mul(log(x, scale), y, scale), scale);
    };

    constexpr intval precision = 8;

    return div(f(x * precision, y * precision, scale * precision),
               precision, 1);
  }

  static_assert(pow(200, 200, 100) < 406, "2 * 2 != 4");
  static_assert(pow(200, 200, 100) > 394, "2 * 2 != 4");

  constexpr auto sqrt(const intval x, const intval scale) noexcept
      -> intval {

    return div(pow(x * 2, scale, scale * 2), 2, 1);
  }

  static_assert(sqrt(400, 100) == 200, "sqrt(4) != 2");

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

    const auto index = div(x, scale, _sin_table_size);
    const auto value = sin_table(index);

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
    //  NOT IMPLEMENTED
    return 0;
  }

  constexpr auto acos(const intval x, const intval scale) noexcept
      -> intval {

    return pi(scale) / 2 - asin(x, scale);
  }

  constexpr auto atan(const intval x, const intval scale) noexcept
      -> intval {
    //  NOT IMPLEMENTED
    return 0;
  }

  constexpr auto atan2(const intval y, const intval x,
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
