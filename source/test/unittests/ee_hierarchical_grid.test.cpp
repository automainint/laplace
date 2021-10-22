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
  namespace grid = eval::grid;

  using std::array, engine::vec2z, eval::hierarchical_grid::map_info,
      eval::hierarchical_grid::pivot,
      eval::hierarchical_grid::process,
      eval::hierarchical_grid::block_of,
      eval::hierarchical_grid::generate,
      eval::hierarchical_grid::pivot_order;

  TEST(engine, eval_hierarchical_grid_none) {
    constexpr auto width  = 5;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 1, 1, //
                                               1, 1, 1, 1, 1 };

    auto info = generate({ 3, 3 }, grid::is_zero, 10,
                         grid::area_of({ width, height }, map));

    EXPECT_TRUE(info.pivots.empty());
  }

  TEST(engine, eval_hierarchical_grid_one) {
    constexpr auto width  = 5;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 1, 1, 1, 1, 1, //
                                               1, 0, 0, 0, 1, //
                                               1, 1, 1, 1, 1 };

    auto info = generate({ 3, 3 }, grid::is_zero, 10,
                         grid::area_of({ width, height }, map));

    EXPECT_EQ(info.pivots.size(), 1);

    if (!info.pivots.empty()) {
      EXPECT_EQ(info.pivots[0].block.x(), 1);
      EXPECT_EQ(info.pivots[0].block.y(), 0);
      EXPECT_EQ(info.pivots[0].position.x(), 3);
      EXPECT_EQ(info.pivots[0].position.y(), 1);
      EXPECT_FALSE(info.pivots[0].is_horizontal);
      EXPECT_TRUE(info.pivots[0].is_vertical);
    }
  }

  TEST(engine, eval_hierarchical_grid_two) {
    constexpr auto width  = 3;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 1, 1, 1, //
                                               1, 1, 1, //
                                               0, 1, 1, //
                                               0, 1, 1, //
                                               1, 1, 1 };

    auto info = generate({ 3, 3 }, grid::is_zero, 10,
                         grid::area_of({ width, height }, map));

    EXPECT_EQ(info.pivots.size(), 2);

    if (info.pivots.size() > 0) {
      EXPECT_EQ(info.pivots[0].block.x(), 0);
      EXPECT_EQ(info.pivots[0].block.y(), 0);
      EXPECT_EQ(info.pivots[0].position.x(), 0);
      EXPECT_EQ(info.pivots[0].position.y(), 2);
      EXPECT_FALSE(info.pivots[0].is_horizontal);
      EXPECT_TRUE(info.pivots[0].is_vertical);
    }

    if (info.pivots.size() > 1) {
      EXPECT_EQ(info.pivots[1].block.x(), 0);
      EXPECT_EQ(info.pivots[1].block.y(), 1);
      EXPECT_EQ(info.pivots[1].position.x(), 0);
      EXPECT_EQ(info.pivots[1].position.y(), 3);
      EXPECT_TRUE(info.pivots[1].is_horizontal);
      EXPECT_TRUE(info.pivots[1].is_vertical);
    }
  }

  TEST(engine, eval_hierarchical_grid_pivots_order) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 1, 0, 1, 0, //
                                               1, 1, 0, 1, 1, //
                                               0, 0, 1, 0, 0, //
                                               1, 1, 0, 1, 1, //
                                               0, 1, 0, 1, 0 };

    auto info = generate({ 2, 2 }, grid::is_zero, 10,
                         grid::area_of({ width, height }, map));

    EXPECT_FALSE(info.pivots.empty());

    for (sl::index i = 1; i < info.pivots.size(); i++) {
      EXPECT_TRUE(pivot_order(info.pivots[i - 1], info.pivots[i]));
    }
  }

  TEST(engine, eval_hierarchical_grid_pivots_order2) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 1, 0, 0, 1, 0, 0, //
                                               1, 1, 1, 1, 1, 1, 1, //
                                               0, 1, 1, 0, 1, 1, 0, //
                                               0, 1, 0, 1, 0, 0, 0, //
                                               1, 1, 1, 0, 1, 1, 1, //
                                               0, 1, 1, 0, 1, 1, 0, //
                                               0, 1, 0, 0, 1, 0, 0 };

    auto info = generate({ 3, 3 }, grid::is_zero, 10,
                         grid::area_of({ width, height }, map));

    EXPECT_FALSE(info.pivots.empty());

    for (sl::index i = 1; i < info.pivots.size(); i++) {
      EXPECT_TRUE(pivot_order(info.pivots[i - 1], info.pivots[i]));
    }
  }

  TEST(engine, eval_hierarchical_grid_block_of) {
    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0 };

    auto const info = generate({ 2, 2 }, grid::is_zero, 10,
                               grid::area_of({ width, height }, map));

    auto const a0 = block_of({ 0, 0 }, info);
    auto const a1 = block_of({ 4, 0 }, info);
    auto const a2 = block_of({ 0, 4 }, info);

    auto const b0 = vec2z { 0, 0 };
    auto const b1 = vec2z { 2, 0 };
    auto const b2 = vec2z { 0, 2 };

    EXPECT_EQ(a0, b0);
    EXPECT_EQ(a1, b1);
    EXPECT_EQ(a2, b2);
  }

  TEST(engine, eval_hierarchical_grid_process_block) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 4 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    process(info);

    EXPECT_FALSE(info.pivots[0].neighbors.empty());
    EXPECT_FALSE(info.pivots[1].neighbors.empty());

    if (!info.pivots[0].neighbors.empty()) {
      EXPECT_EQ(info.pivots[0].neighbors[0].node, 1);
    }

    if (!info.pivots[1].neighbors.empty()) {
      EXPECT_EQ(info.pivots[1].neighbors[0].node, 0);
    }
  }

  TEST(engine, eval_hierarchical_grid_process_block2) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               1, 1, 1, 1, 1, 1, 1, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 4 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    process(info);

    EXPECT_TRUE(info.pivots[0].neighbors.empty());
    EXPECT_TRUE(info.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_process_block3) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               1, 1, 1, 0, 0, 0, 0, //
                                               0, 0, 1, 1, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 4, 4 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 2, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    process(info);

    EXPECT_FALSE(info.pivots[0].neighbors.empty());
    EXPECT_FALSE(info.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_process_block4) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               1, 1, 1, 0, 0, 0, 0, //
                                               0, 0, 1, 1, 1, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 4, 4 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 2, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    process(info);

    EXPECT_TRUE(info.pivots[0].neighbors.empty());
    EXPECT_TRUE(info.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_nearest_pivot) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 1, 0, 0, 0, 0, //
                                               0, 0, 1, 0, 0, 0, 0, //
                                               0, 0, 1, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto const info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    auto const n = nearest_pivot({ 3, 3 }, info);

    EXPECT_EQ(n, 0);
  }

  TEST(engine, eval_hierarchical_grid_nearest_pivot2) {
    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto map = array<int8_t, size> { 0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 1, 1, 1, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0, //
                                               0, 0, 0, 0, 0, 0, 0 };

    auto const info = map_info {
      .grid_scale   = 10,
      .grid_area    = grid::area_of({ width, height }, map),
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = grid::is_zero,
      .pivots       = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks       = { sl::vector<sl::index> { 0, 1 } }
    };

    auto const n = nearest_pivot({ 3, 3 }, info);

    EXPECT_EQ(n, 1);
  }
}
