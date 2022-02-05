/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/eval/arithmetic.impl.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::eval::impl::wrap_add, engine::eval::impl::wrap_sub,
      engine::eval::impl::_div, engine::eval::impl::_mul,
      engine::eval::impl::_sub, engine::eval::impl::div_sum,
      engine::eval::impl::_int_min, engine::eval::impl::_int_max;

  TEST(engine, eval_arithmetic_wraps) {
    EXPECT_EQ(wrap_add((int8_t) 2, (int8_t) 3), (int8_t) 5);
    EXPECT_EQ(wrap_add((int32_t) 2, (int32_t) 3), (int32_t) 5);
    EXPECT_EQ(wrap_add((int64_t) 2, (int64_t) 3), (int64_t) 5);
    EXPECT_EQ(wrap_sub((int8_t) 5, (int8_t) 3), (int8_t) 2);
    EXPECT_EQ(wrap_sub((int32_t) 5, (int32_t) 3), (int32_t) 2);
    EXPECT_EQ(wrap_sub((int64_t) 5, (int64_t) 3), (int64_t) 2);
  }

  TEST(engine, eval_arithmetic_wraps_overflow) {
    EXPECT_EQ(wrap_add((int8_t) 127, (int8_t) 1), (int8_t) -128);
    EXPECT_EQ(wrap_add((int32_t) 2147483647, (int32_t) 1),
              (int32_t) -2147483648);
    EXPECT_EQ(
        wrap_add((int64_t) 9'223'372'036'854'775'807, (int64_t) 1),
        (int64_t) -9'223'372'036'854'775'808);
    EXPECT_EQ(wrap_sub((int8_t) -128, (int8_t) 1), (int8_t) 127);
    EXPECT_EQ(wrap_sub((int32_t) -2147483648, (int32_t) 1),
              (int32_t) 2147483647);
    EXPECT_EQ(
        wrap_sub((int64_t) -9'223'372'036'854'775'808, (int64_t) 1),
        (int64_t) 9'223'372'036'854'775'807);
  }

  TEST(engine, eval_arithmetic_safe_div) {
    EXPECT_EQ(_div(4, 2), 2);
    EXPECT_EQ(_div(5, 2), 2);
    EXPECT_EQ(_div(4, 0), _int_max);
    EXPECT_EQ(_div(-4, 0), _int_min);
    EXPECT_EQ(_div(0, 0), 1);
  }

  TEST(engine, eval_arithmetic_safe_mul) {
    EXPECT_EQ(_mul(0x10000, 0x10000), 0x100000000);
    EXPECT_EQ(_mul(-0x10000, 0x10000), -0x100000000);
    EXPECT_EQ(_mul(0x10000, -0x10000), -0x100000000);
    EXPECT_EQ(_mul(-0x10000, -0x10000), 0x100000000);
    EXPECT_EQ(_mul(0x100000000, 0x100), 0x10000000000);
    EXPECT_EQ(_mul(-0x100000000, 0x100), -0x10000000000);
    EXPECT_EQ(_mul(0x100000000, -0x100), -0x10000000000);
    EXPECT_EQ(_mul(-0x100000000, -0x100), 0x10000000000);
    EXPECT_EQ(_mul(0x1000000000000000, 0x1000000000000000), _int_max);
    EXPECT_EQ(_mul(-0x1000000000000000, 0x1000000000000000),
              _int_min);
    EXPECT_EQ(_mul(0x1000000000000000, -0x1000000000000000),
              _int_min);
    EXPECT_EQ(_mul(-0x1000000000000000, -0x1000000000000000),
              _int_max);
  }

  TEST(engine, eval_arithmetic_safe_div_sum) {
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

  TEST(engine, eval_arithmetic_safe_sub) {
    EXPECT_EQ(_sub(1), -1);
    EXPECT_EQ(_sub(_int_max), -_int_max);
    EXPECT_EQ(_sub(_int_min), _int_max);
    EXPECT_EQ(_sub(5, 3), 2);
    EXPECT_EQ(_sub(5, _int_min), _int_max);
    EXPECT_EQ(_sub(-5, _int_min), _int_max - 4);
  }
}
