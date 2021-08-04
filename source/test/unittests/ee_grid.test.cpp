/*  test/unittests/ee_astar.test.cpp
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

#include "../../laplace/engine/eval/grid.h"
#include <gtest/gtest.h>

namespace laplace::test {
  namespace grid = engine::eval::grid;

  TEST(engine, eval_grid_search_straigth) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto v = grid::path_search({ width, height }, 10, map,
                               [](const int8_t x) {
                                 return x == 0;
                               },
                               { 1, 1 }, { 3, 3 });

    EXPECT_EQ(v.size(), 2u);
  }

  TEST(engine, eval_grid_search_complex) {
    constexpr auto width  = 7;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, 1, 1, //
                                               1, 0, 0, 1, 0, 0, 1, //
                                               1, 0, 0, 1, 0, 0, 1, //
                                               1, 0, 0, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1, 1, 1 };

    auto v = grid::path_search({ width, height }, 10, map,
                               [](const int8_t x) {
                                 return x == 0;
                               },
                               { 1, 1 }, { 5, 1 });

    EXPECT_EQ(v.size(), 3u);
  }
}
