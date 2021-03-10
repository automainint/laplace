/*  test/unittests/m_traits.test.cpp
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

#include "../../laplace/math/complex.h"
#include "../../laplace/math/matrix.h"
#include "../../laplace/math/quaternion.h"
#include "../../laplace/math/shortnames.h"
#include "../../laplace/math/traits.h"
#include "../../laplace/math/vector.h"
#include <gtest/gtest.h>
#include <random>

namespace laplace::test {
  using math::get_unit, math::get_zero, math::get_size,
      math::get, math::set;

  using namespace math::shortnames;

  TEST(math, traits_size) {
    constexpr auto float_size = get_size<float>();
    constexpr auto vec2_size  = get_size<vec2>();
    constexpr auto vec3_size  = get_size<vec3>();
    constexpr auto mat2_size  = get_size<mat2>();
    constexpr auto mat4_size  = get_size<mat4>();
    constexpr auto comp_size  = get_size<comp>();
    constexpr auto quat_size  = get_size<quat>();

    EXPECT_EQ(float_size, 1u);
    EXPECT_EQ(vec2_size, 2u);
    EXPECT_EQ(vec3_size, 3u);
    EXPECT_EQ(mat2_size, 4u);
    EXPECT_EQ(mat4_size, 16u);
    EXPECT_EQ(comp_size, 2u);
    EXPECT_EQ(quat_size, 4u);
  }

  TEST(math, traits_get_constexpr) {
    constexpr auto float_x = get(1., 0);
    constexpr auto vec2_x = get(get_unit<vec2>(), 0);
    constexpr auto vec3_x = get(get_unit<vec3>(), 0);
    constexpr auto mat2_x = get(get_unit<mat2>(), 0);
    constexpr auto mat4_x = get(get_unit<mat4>(), 0);
    constexpr auto comp_x = get(get_unit<comp>(), 0);
    constexpr auto quat_x = get(get_unit<quat>(), 0);
    constexpr auto quat_w = get(get_unit<quat>(), 3);

    EXPECT_EQ(float_x, 1.);
    EXPECT_EQ(vec2_x, 1.);
    EXPECT_EQ(vec3_x, 1.);
    EXPECT_EQ(mat2_x, 1.);
    EXPECT_EQ(mat4_x, 1.);
    EXPECT_EQ(comp_x, 1.);
    EXPECT_EQ(quat_x, 0.);
    EXPECT_EQ(quat_w, 1.);
  }

  TEST(math, traits_get) {
    auto float_x = get(1.f, 0);
    auto vec2_x  = get(get_unit<vec2>(), 0);
    auto vec3_x  = get(get_unit<vec2>(), 0);
    auto mat2_x  = get(get_unit<mat4>(), 0);
    auto mat4_x  = get(get_unit<mat4>(), 0);
    auto comp_x  = get(get_unit<comp>(), 0);
    auto quat_x  = get(get_unit<quat>(), 0);
    auto quat_w  = get(get_unit<quat>(), 3);

    EXPECT_EQ(float_x, 1.);
    EXPECT_EQ(vec2_x, 1.);
    EXPECT_EQ(vec3_x, 1.);
    EXPECT_EQ(mat2_x, 1.);
    EXPECT_EQ(mat4_x, 1.);
    EXPECT_EQ(comp_x, 1.);
    EXPECT_EQ(quat_x, 0.);
    EXPECT_EQ(quat_w, 1.);
  }

  TEST(math, traits_set) {
    float x  = {};
    vec2  v2 = {};
    vec3  v3 = {};
    mat2  m2 = {};
    mat4  m4 = {};
    comp  c  = {};
    quat  q  = {};

    set(x, 0, 1.);

    set(v2, 0, 1.);
    set(v2, 1, 2.);

    set(v3, 0, 1.);
    set(v3, 1, 2.);
    set(v3, 2, 3.);

    set(m2, 0, 1.);
    set(m2, 1, 2.);
    set(m2, 2, 3.);
    set(m2, 3, 4.);

    set(m4, 0, 1.);
    set(m4, 5, 2.);
    set(m4, 10, 3.);
    set(m4, 15, 4.);

    set(c, 0, 1.);
    set(c, 1, 2.);

    set(q, 0, 1.);
    set(q, 1, 2.);
    set(q, 2, 3.);
    set(q, 3, 4.);

    EXPECT_EQ(get(x, 0), 1.);
    EXPECT_EQ(get(v2, 0), 1.);
    EXPECT_EQ(get(v2, 1), 2.);
    EXPECT_EQ(get(v3, 0), 1.);
    EXPECT_EQ(get(v3, 1), 2.);
    EXPECT_EQ(get(v3, 2), 3.);
    EXPECT_EQ(get(m2, 0), 1.);
    EXPECT_EQ(get(m2, 1), 2.);
    EXPECT_EQ(get(m2, 2), 3.);
    EXPECT_EQ(get(m2, 3), 4.);
    EXPECT_EQ(get(m4, 0), 1.);
    EXPECT_EQ(get(m4, 5), 2.);
    EXPECT_EQ(get(m4, 10), 3.);
    EXPECT_EQ(get(m4, 15), 4.);
    EXPECT_EQ(get(c, 0), 1.);
    EXPECT_EQ(get(c, 1), 2.);
    EXPECT_EQ(get(q, 0), 1.);
    EXPECT_EQ(get(q, 1), 2.);
    EXPECT_EQ(get(q, 2), 3.);
    EXPECT_EQ(get(q, 3), 4.);
  }
}
