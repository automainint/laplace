/*  test/unittests/ee_hierarchical_grid.test.cpp
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

#include "../../laplace/engine/eval/hierarchical_grid.h"
#include <array>
#include <gtest/gtest.h>

namespace laplace::test {
  namespace eval = engine::eval;

  using std::array;

  TEST(engine, eval_hierarchical_grid_trivial) {
    constexpr auto width  = 2;
    constexpr auto height = 2;
    constexpr auto scale  = 10;
    constexpr auto size   = width * height;
    constexpr auto map    = array<int8_t, size> { 0, 0, 0, 0 };

    auto hmap = eval::hierarchical_grid::gen(
        { width, height }, scale, map,
        [](const int8_t x) { return x <= 0; }, { width, height });

    EXPECT_EQ(hmap.empty(), 0);
  }
}
