/*  test/unittests/ee_grid.test.cpp
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
  namespace grid  = engine::eval::grid;
  namespace astar = engine::eval::astar;

  using engine::vec2z;

  TEST(engine, eval_grid_path_exists) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto f = grid::path_exists({ 1, 1 }, { 3, 3 }, grid::is_zero,
                               grid::area_of({ width, height }, map));

    EXPECT_TRUE(f);
  }

  TEST(engine, eval_grid_path_not_exists) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto f = grid::path_exists({ 3, 1 }, { 1, 3 }, grid::is_zero,
                               grid::area_of({ width, height }, map));

    EXPECT_FALSE(f);
  }

  TEST(engine, eval_grid_path_exists_submap) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto f = grid::path_exists(
        { 1, 1 }, { 3, 2 }, grid::is_zero,
        grid::submap({ 1, 1 }, { 4, 3 },
                     grid::area_of({ width, height }, map)));

    EXPECT_TRUE(f);
  }

  TEST(engine, eval_grid_path_not_exists_edge) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, //
                                               1, 1, 1, 1, 1, //
                                               0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0 };

    auto f = grid::path_exists({ 2, 0 }, { 2, 4 }, grid::is_zero,
                               grid::area_of({ width, height }, map));

    EXPECT_FALSE(f);
  }

  TEST(engine, eval_grid_path_not_exists_submap) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto f = grid::path_exists(
        { 3, 1 }, { 1, 3 }, grid::is_zero,
        grid::submap({ 1, 1 }, { 3, 3 },
                     grid::area_of({ width, height }, map)));

    EXPECT_FALSE(f);
  }

  TEST(engine, eval_grid_search_straigth) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto state = grid::path_search_init(
        { 1, 1 }, { 3, 3 }, grid::is_zero, 10,
        grid::area_of({ width, height }, map));

    auto s = astar::status {};

    do {
      s = grid::path_search_loop(state);
    } while (s == astar::status::progress);

    const auto v = grid::path_search_finish(state);

    EXPECT_EQ(s, astar::status::success);
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

    auto state = grid::path_search_init(
        { 1, 1 }, { 5, 1 }, grid::is_zero, 10,
        grid::area_of({ width, height }, map));

    auto s = astar::status {};

    do {
      s = grid::path_search_loop(state);
    } while (s == astar::status::progress);

    const auto v = grid::path_search_finish(state);

    EXPECT_EQ(s, astar::status::success);
    EXPECT_EQ(v.size(), 3u);
  }

  TEST(engine, eval_grid_search_partial) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1, 0, 1, //
                                               1, 0, 0, 0, 1, 0, 1, //
                                               1, 0, 0, 0, 1, 0, 1, //
                                               1, 0, 0, 0, 1, 0, 1, //
                                               1, 1, 1, 1, 1, 1, 1 };

    auto state = grid::path_search_init(
        { 1, 5 }, { 5, 1 }, grid::is_zero, 10,
        grid::area_of({ width, height }, map));

    auto s = astar::status {};

    do {
      s = grid::path_search_loop(state);
    } while (s == astar::status::progress);

    const auto v = grid::path_search_finish(state);

    EXPECT_EQ(s, astar::status::failed);
    EXPECT_EQ(v.size(), 2u);

    if (v.size() > 1) {
      EXPECT_EQ(v[1].x(), 3);
      EXPECT_EQ(v[1].y(), 3);
    }
  }

  TEST(engine, eval_grid_search_complex_submap) {
    constexpr auto width  = 7;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr std::array<int8_t, size> map = { 1, 1, 1, 1, 1, 1, 1, //
                                               1, 0, 0, 1, 0, 0, 1, //
                                               1, 0, 0, 1, 0, 0, 1, //
                                               1, 0, 0, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1, 1, 1 };

    auto state = grid::path_search_init(
        { 2, 1 }, { 4, 1 }, grid::is_zero, 10,
        grid::submap({ 2, 1 }, { 5, 4 },
                     grid::area_of({ width, height }, map)));

    auto s = astar::status {};

    do {
      s = grid::path_search_loop(state);
    } while (s == astar::status::progress);

    auto const v = grid::path_search_finish(state);

    EXPECT_EQ(s, astar::status::success);
    EXPECT_EQ(v.size(), 5u);
  }

  TEST(engine, eval_grid_convolve_empty) {
    constexpr auto width     = 3;
    constexpr auto height    = 3;
    constexpr auto size      = width * height;
    constexpr auto fp_width  = 3;
    constexpr auto fp_height = 3;
    constexpr auto center_x  = 1;
    constexpr auto center_y  = 1;

    constexpr auto map = std::array<int8_t, size> { 0, 0, 0, //
                                                    0, 0, 0, //
                                                    0, 0, 0 };

    constexpr auto footprint = std::array<int8_t, size> { 0, 0, 0, //
                                                          0, 0, 0, //
                                                          0, 0, 0 };

    auto res = std::array<int8_t, size> { 0, 0, 0, //
                                          0, 0, 0, //
                                          0, 0, 0 };

    auto dst = std::array<int8_t, size> {};

    grid::convolve({ width, height }, dst, map,
                   { fp_width, fp_height }, { center_x, center_y },
                   footprint);

    EXPECT_EQ(dst, res);
  }

  TEST(engine, eval_grid_convolve_single) {
    constexpr auto width     = 3;
    constexpr auto height    = 3;
    constexpr auto size      = width * height;
    constexpr auto fp_width  = 3;
    constexpr auto fp_height = 3;
    constexpr auto center_x  = 1;
    constexpr auto center_y  = 1;

    constexpr auto map = std::array<int8_t, size> { 0, 0, 0, //
                                                    0, 1, 0, //
                                                    0, 0, 0 };

    constexpr auto footprint = std::array<int8_t, size> { 1, 0, 0, //
                                                          1, 0, 1, //
                                                          1, 0, 1 };

    auto res = std::array<int8_t, size> { 1, 0, 0, //
                                          1, 0, 1, //
                                          1, 0, 1 };

    auto dst = std::array<int8_t, size> {};

    grid::convolve({ width, height }, dst, map,
                   { fp_width, fp_height }, { center_x, center_y },
                   footprint);

    EXPECT_EQ(dst, res);
  }

  TEST(engine, eval_grid_convolve_multiple) {
    constexpr auto width     = 3;
    constexpr auto height    = 3;
    constexpr auto size      = width * height;
    constexpr auto fp_width  = 3;
    constexpr auto fp_height = 3;
    constexpr auto center_x  = 1;
    constexpr auto center_y  = 1;

    constexpr auto map = std::array<int8_t, size> { 1, 1, 1, //
                                                    1, 0, 1, //
                                                    1, 1, 1 };

    constexpr auto footprint = std::array<int8_t, size> { 0, 0, 0, //
                                                          0, 1, 0, //
                                                          0, 0, 0 };

    auto res = std::array<int8_t, size> { 1, 1, 1, //
                                          1, 0, 1, //
                                          1, 1, 1 };

    auto dst = std::array<int8_t, size> {};

    grid::convolve({ width, height }, dst, map,
                   { fp_width, fp_height }, { center_x, center_y },
                   footprint);

    EXPECT_EQ(dst, res);
  }

  TEST(engine, eval_grid_convolve_offset) {
    constexpr auto width     = 3;
    constexpr auto height    = 3;
    constexpr auto size      = width * height;
    constexpr auto fp_width  = 3;
    constexpr auto fp_height = 3;
    constexpr auto center_x  = 1;
    constexpr auto center_y  = 1;

    constexpr auto map = std::array<int8_t, size> { 1, 1, 1, //
                                                    1, 0, 1, //
                                                    1, 1, 1 };

    constexpr auto footprint = std::array<int8_t, size> { 0, 0, 0, //
                                                          0, 0, 1, //
                                                          0, 0, 0 };

    auto res = std::array<int8_t, size> { 0, 1, 1, //
                                          0, 1, 0, //
                                          0, 1, 1 };

    auto dst = std::array<int8_t, size> {};

    grid::convolve({ width, height }, dst, map,
                   { fp_width, fp_height }, { center_x, center_y },
                   footprint);

    EXPECT_EQ(dst, res);
  }

  TEST(engine, eval_grid_nearest_empty) {
    constexpr auto width  = 3;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr auto map = std::array<int8_t, size> { 0, 0, 0, //
                                                    0, 0, 0, //
                                                    0, 0, 0 };

    constexpr auto s = vec2z { width, height };
    constexpr auto p = vec2z { 1, 1 };

    EXPECT_EQ(grid::nearest(p, s, map), p);
  }

  TEST(engine, eval_grid_nearest_complex) {
    constexpr auto width  = 4;
    constexpr auto height = 4;
    constexpr auto size   = width * height;

    constexpr auto map = std::array<int8_t, size> { 1, 1, 1, 0, //
                                                    1, 1, 1, 0, //
                                                    1, 1, 0, 0, //
                                                    0, 0, 0, 0 };

    constexpr auto s = vec2z { width, height };
    constexpr auto p = vec2z { 1, 1 };
    constexpr auto r = vec2z { 2, 2 };

    EXPECT_EQ(grid::nearest(p, s, map), r);
  }
}
