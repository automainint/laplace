/*  test/unittests/ee_integral.test.cpp
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

#include "../../laplace/engine/eval/integral.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::intval, engine::eval::constant_scale,
      engine::eval::add, engine::eval::sub, engine::eval::div,
      engine::eval::mul, engine::eval::sqr, engine::eval::div_sum,
      engine::eval::e, engine::eval::log2e, engine::eval::log10e,
      engine::eval::pi, engine::eval::inv_pi,
      engine::eval::invsqrt_pi, engine::eval::ln2, engine::eval::ln10,
      engine::eval::sqrt2, engine::eval::sqrt3,
      engine::eval::inv_sqrt3, engine::eval::egamma,
      engine::eval::phi, engine::eval::exp, engine::eval::log,
      engine::eval::log2, engine::eval::log10, engine::eval::pow,
      engine::eval::sqrt, engine::eval::sin, engine::eval::cos;

  static constexpr auto _int_max = std::numeric_limits<intval>::max();
  static constexpr auto _int_min = std::numeric_limits<intval>::min();

  TEST(engine, eval_integral_constant_scale) {
    EXPECT_GT(constant_scale(), 0);
  }

  TEST(engine, eval_integral_add) {
    EXPECT_EQ(add(2, 3), 5);
  }

  TEST(engine, eval_integral_sub) {
    EXPECT_EQ(sub(5, 2), 3);
  }

  TEST(engine, eval_integral_mul) {
    EXPECT_EQ(mul(0x10000, 0x10000, 1), 0x100000000);
    EXPECT_EQ(mul(-0x10000, 0x10000, 1), -0x100000000);
    EXPECT_EQ(mul(0x10000, -0x10000, 1), -0x100000000);
    EXPECT_EQ(mul(-0x10000, -0x10000, 1), 0x100000000);
    EXPECT_EQ(mul(0x100000000, 0x100, 1), 0x10000000000);
    EXPECT_EQ(mul(-0x100000000, 0x100, 1), -0x10000000000);
    EXPECT_EQ(mul(0x100000000, -0x100, 1), -0x10000000000);
    EXPECT_EQ(mul(-0x100000000, -0x100, 1), 0x10000000000);
    EXPECT_EQ(mul(0x1000000000000000, 0x1000000000000000, 1),
              _int_max);
    EXPECT_EQ(mul(-0x1000000000000000, 0x1000000000000000, 1),
              _int_min);
    EXPECT_EQ(mul(0x1000000000000000, -0x1000000000000000, 1),
              _int_min);
    EXPECT_EQ(mul(-0x1000000000000000, -0x1000000000000000, 1),
              _int_max);
  }

  TEST(engine, eval_integral_div) {
    EXPECT_EQ(div(15, 10, 100), 150);
    EXPECT_EQ(div(150, 100, 10), 15);
    EXPECT_EQ(div(14, 10, 1), 1);
    EXPECT_EQ(div(15, 10, 1), 2);
    EXPECT_EQ(div(24, 10, 1), 2);
    EXPECT_EQ(div(-14, 10, 1), -1);
    EXPECT_EQ(div(-15, 10, 1), -2);
    EXPECT_EQ(div(-24, 10, 1), -2);
    EXPECT_EQ(div(0x100000000000, 0x100000000000, 1000), 1000);
    EXPECT_EQ(div(100000000, 10000000000000000, 100000000), 1);
    EXPECT_EQ(div(100000000, 100000000, 10000000000000000),
              10000000000000000);
    EXPECT_EQ(div(1, 0, 1), _int_max);
    EXPECT_EQ(div(-1, 0, 1), _int_min);
    EXPECT_EQ(div(0, 0, 1), 1);
    EXPECT_EQ(div(0, 0, 100), 100);
    EXPECT_EQ(div(0x10000000000000, 1, 0x10000000000000), _int_max);
    EXPECT_EQ(
        div(0x10000000000000, 0x10000000000000, 0x10000000000000),
        0x10000000000000);
  }

  TEST(engine, eval_integral_sqr) {
    EXPECT_EQ(sqr(-1), 1);
    EXPECT_EQ(sqr(2), 4);
    EXPECT_EQ(sqr(100), 10000);
    EXPECT_EQ(sqr(-10, 10), 10);
    EXPECT_EQ(sqr(20, 10), 40);
    EXPECT_EQ(sqr(1000, 10), 100000);
  }

  TEST(engine, eval_integral_div_sum) {
    EXPECT_EQ(div_sum(100, 200, 3), 100);
    EXPECT_EQ(div_sum(100, 200, 300, 6), 100);
    EXPECT_EQ(div_sum(-100, -200, 3), -100);
    EXPECT_EQ(div_sum(-100, -200, -300, 6), -100);

    EXPECT_EQ(div_sum(0x7000000000000000, 0x7000000000000000,
                      0x100000000000000),
              0xe0);
    EXPECT_EQ(div_sum(0x7000000000000000, 0x7000000000000000,
                      0x7000000000000000, 0x100000000000000),
              0x150);
    EXPECT_EQ(div_sum(-0x7000000000000000, -0x7000000000000000,
                      0x100000000000000),
              -0xe0);
    EXPECT_EQ(div_sum(-0x7000000000000000, -0x7000000000000000,
                      -0x7000000000000000, 0x100000000000000),
              -0x150);
  }

  TEST(engine, eval_integral_constants) {
    EXPECT_EQ(e(1000000), 2718282);
    EXPECT_EQ(log2e(1000000), 1442695);
    EXPECT_EQ(log10e(1000000), 434294);
    EXPECT_EQ(pi(1000000), 3141593);
    EXPECT_EQ(inv_pi(1000000), 318310);
    EXPECT_EQ(invsqrt_pi(1000000), 564190);
    EXPECT_EQ(ln2(1000000), 693147);
    EXPECT_EQ(ln10(1000000), 2302585);
    EXPECT_EQ(sqrt2(1000000), 1414214);
    EXPECT_EQ(sqrt3(1000000), 1732051);
    EXPECT_EQ(inv_sqrt3(1000000), 577350);
    EXPECT_EQ(egamma(1000000), 577216);
    EXPECT_EQ(phi(1000000), 1618034);
  }

  TEST(engine, eval_integral_exp) {
    EXPECT_EQ(exp(0, 100), 100);
    EXPECT_EQ(exp(100, 100), e(100));
  }

  TEST(engine, eval_integral_log) {
    EXPECT_EQ(log(e(100), 100), 100);
    EXPECT_EQ(log((e(100) * e(100)) / 100, 100), 200);
  }

  TEST(engine, eval_integral_log2) {
    EXPECT_EQ(log2(200, 100), 100);
    EXPECT_EQ(log2(400, 100), 200);
  }

  TEST(engine, eval_integral_log10) {
    EXPECT_EQ(log10(100, 10), 10);
    EXPECT_EQ(log10(10000, 100), 200);
    EXPECT_EQ(log10(100000, 100), 300);
  }

  TEST(engine, eval_integral_exp_log) {
    EXPECT_LT(exp(log(200, 100), 100), 202);
    EXPECT_GT(exp(log(200, 100), 100), 198);
  }

  TEST(engine, eval_integral_pow) {
    EXPECT_LT(pow(200, 200, 100), 402);
    EXPECT_GT(pow(200, 200, 100), 398);
  }

  TEST(engine, eval_integral_sqrt) {
    EXPECT_EQ(sqrt(400, 100), 200);
    EXPECT_EQ(sqrt(9, 1), 3);
    EXPECT_EQ(sqrt(81, 1), 9);
    EXPECT_EQ(sqrt(10000, 1), 100);
  }

  TEST(engine, eval_sin_cos) {
    EXPECT_LT(mul(sin(10, 100), sin(10, 100), 100) +
                  mul(cos(10, 100), cos(10, 100), 100),
              102);

    EXPECT_GT(mul(sin(10, 100), sin(10, 100), 100) +
                  mul(cos(10, 100), cos(10, 100), 100),
              98);

    EXPECT_LT(mul(sin(20, 100), sin(20, 100), 100) +
                  mul(cos(20, 100), cos(20, 100), 100),
              102);

    EXPECT_GT(mul(sin(20, 100), sin(20, 100), 100) +
                  mul(cos(20, 100), cos(20, 100), 100),
              98);

    EXPECT_LT(mul(sin(90, 100), sin(90, 100), 100) +
                  mul(cos(90, 100), cos(90, 100), 100),
              102);

    EXPECT_GT(mul(sin(90, 100), sin(90, 100), 100) +
                  mul(cos(90, 100), cos(90, 100), 100),
              98);

    EXPECT_LT(mul(sin(130, 100), sin(130, 100), 100) +
                  mul(cos(130, 100), cos(130, 100), 100),
              102);

    EXPECT_GT(mul(sin(130, 100), sin(130, 100), 100) +
                  mul(cos(130, 100), cos(130, 100), 100),
              98);
  }
}
