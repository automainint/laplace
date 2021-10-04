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

  using std::array, engine::vec2z;

  TEST(engine, eval_hierarchical_grid_none) {
    using eval::hierarchical_grid::generate;

    constexpr auto width  = 5;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> { 1, 1, 1, 1, 1, //
                                                1, 0, 0, 1, 1, //
                                                1, 1, 1, 1, 1 };

    auto map = generate({ 3, 3 }, { width, height }, 10, grid,
                        [](int8_t const x) { return x <= 0; });

    EXPECT_TRUE(map.pivots.empty());
  }

  TEST(engine, eval_hierarchical_grid_one) {
    using eval::hierarchical_grid::generate;

    constexpr auto width  = 5;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> { 1, 1, 1, 1, 1, //
                                                1, 0, 0, 0, 1, //
                                                1, 1, 1, 1, 1 };

    auto map = generate({ 3, 3 }, { width, height }, 10, grid,
                        [](int8_t const x) { return x <= 0; });

    EXPECT_EQ(map.pivots.size(), 1);

    if (!map.pivots.empty()) {
      EXPECT_EQ(map.pivots[0].block.x(), 1);
      EXPECT_EQ(map.pivots[0].block.y(), 0);
      EXPECT_EQ(map.pivots[0].position.x(), 3);
      EXPECT_EQ(map.pivots[0].position.y(), 1);
      EXPECT_FALSE(map.pivots[0].is_horizontal);
      EXPECT_TRUE(map.pivots[0].is_vertical);
    }
  }

  TEST(engine, eval_hierarchical_grid_two) {
    using eval::hierarchical_grid::generate;

    constexpr auto width  = 3;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> { 1, 1, 1, //
                                                1, 1, 1, //
                                                0, 1, 1, //
                                                0, 1, 1, //
                                                1, 1, 1 };

    auto map = generate({ 3, 3 }, { width, height }, 10, grid,
                        [](int8_t const x) { return x <= 0; });

    EXPECT_EQ(map.pivots.size(), 2);

    if (map.pivots.size() >= 0) {
      EXPECT_EQ(map.pivots[0].block.x(), 0);
      EXPECT_EQ(map.pivots[0].block.y(), 0);
      EXPECT_EQ(map.pivots[0].position.x(), 0);
      EXPECT_EQ(map.pivots[0].position.y(), 2);
      EXPECT_FALSE(map.pivots[0].is_horizontal);
      EXPECT_TRUE(map.pivots[0].is_vertical);
    }

    if (map.pivots.size() >= 1) {
      EXPECT_EQ(map.pivots[1].block.x(), 0);
      EXPECT_EQ(map.pivots[1].block.y(), 1);
      EXPECT_EQ(map.pivots[1].position.x(), 0);
      EXPECT_EQ(map.pivots[1].position.y(), 3);
      EXPECT_TRUE(map.pivots[1].is_horizontal);
      EXPECT_TRUE(map.pivots[1].is_vertical);
    }
  }

  TEST(engine, eval_hierarchical_grid_pivots_order) {
    using eval::hierarchical_grid::generate;
    using eval::hierarchical_grid::pivot_order;

    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> { 0, 1, 0, 1, 0, //
                                                1, 1, 0, 1, 1, //
                                                0, 0, 1, 0, 0, //
                                                1, 1, 0, 1, 1, //
                                                0, 1, 0, 1, 0 };

    auto map = generate({ 2, 2 }, { width, height }, 10, grid,
                        [](int8_t const x) { return x <= 0; });

    EXPECT_FALSE(map.pivots.empty());

    for (sl::index i = 1; i < map.pivots.size(); i++) {
      EXPECT_TRUE(pivot_order(map.pivots[i - 1], map.pivots[i]));
    }
  }

  TEST(engine, eval_hierarchical_grid_pivots_order2) {
    using eval::hierarchical_grid::generate;
    using eval::hierarchical_grid::pivot_order;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 1, 0, 0, 1, 0, 0, //
      1, 1, 1, 1, 1, 1, 1, //
      0, 1, 1, 0, 1, 1, 0, //
      0, 1, 0, 1, 0, 0, 0, //
      1, 1, 1, 0, 1, 1, 1, //
      0, 1, 1, 0, 1, 1, 0, //
      0, 1, 0, 0, 1, 0, 0
    };

    auto map = generate({ 3, 3 }, { width, height }, 10, grid,
                        [](int8_t const x) { return x <= 0; });

    EXPECT_FALSE(map.pivots.empty());

    for (sl::index i = 1; i < map.pivots.size(); i++) {
      EXPECT_TRUE(pivot_order(map.pivots[i - 1], map.pivots[i]));
    }
  }

  TEST(engine, eval_hierarchical_grid_block_of) {
    using eval::hierarchical_grid::block_of;
    using eval::hierarchical_grid::generate;

    constexpr auto width  = 5;
    constexpr auto height = 5;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> { 0, 0, 0, 0, 0, //
                                                0, 0, 0, 0, 0, //
                                                0, 0, 0, 0, 0, //
                                                0, 0, 0, 0, 0, //
                                                0, 0, 0, 0, 0 };

    auto const map = generate({ 2, 2 }, { width, height }, 10, grid,
                              [](int8_t const x) { return x <= 0; });

    auto const a0 = block_of({ 0, 0 }, map);
    auto const a1 = block_of({ 4, 0 }, map);
    auto const a2 = block_of({ 0, 4 }, map);

    auto const b0 = vec2z { 0, 0 };
    auto const b1 = vec2z { 2, 0 };
    auto const b2 = vec2z { 0, 2 };

    EXPECT_EQ(a0, b0);
    EXPECT_EQ(a1, b1);
    EXPECT_EQ(a2, b2);
  }

  TEST(engine, eval_hierarchical_grid_process_block) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::pivot;
    using eval::hierarchical_grid::process;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots= { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 4 },
                          .is_vertical = true } },
      .blocks= { sl::vector<sl::index> { 0, 1 } }
    };

    process(map);

    EXPECT_FALSE(map.pivots[0].neighbors.empty());
    EXPECT_FALSE(map.pivots[1].neighbors.empty());

    if (!map.pivots[0].neighbors.empty()) {
      EXPECT_EQ(map.pivots[0].neighbors[0].node, 1);
    }

    if (!map.pivots[1].neighbors.empty()) {
      EXPECT_EQ(map.pivots[1].neighbors[0].node, 0);
    }
  }

  TEST(engine, eval_hierarchical_grid_process_block2) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::pivot;
    using eval::hierarchical_grid::process;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      1, 1, 1, 1, 1, 1, 1, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots= { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 4 },
                          .is_vertical = true } },
      .blocks= { sl::vector<sl::index> { 0, 1 } }
    };

    process(map);

    EXPECT_TRUE(map.pivots[0].neighbors.empty());
    EXPECT_TRUE(map.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_process_block3) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::pivot;
    using eval::hierarchical_grid::process;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      1, 1, 1, 0, 0, 0, 0, //
      0, 0, 1, 1, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 4, 4 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots= { pivot { .block         = { 0, 0 },
                          .position      = { 2, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks= { sl::vector<sl::index> { 0, 1 } }
    };

    process(map);

    EXPECT_FALSE(map.pivots[0].neighbors.empty());
    EXPECT_FALSE(map.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_process_block4) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::pivot;
    using eval::hierarchical_grid::process;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      1, 1, 1, 0, 0, 0, 0, //
      0, 0, 1, 1, 1, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 4, 4 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots= { pivot { .block         = { 0, 0 },
                          .position      = { 2, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks= { sl::vector<sl::index> { 0, 1 } }
    };

    process(map);

    EXPECT_TRUE(map.pivots[0].neighbors.empty());
    EXPECT_TRUE(map.pivots[1].neighbors.empty());
  }

  TEST(engine, eval_hierarchical_grid_nearest_pivot) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::nearest_pivot;
    using eval::hierarchical_grid::pivot;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 1, 0, 0, 0, 0, //
      0, 0, 1, 0, 0, 0, 0, //
      0, 0, 1, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto const map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots      = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks      = { sl::vector<sl::index> { 0, 1 } }
    };

    auto const n = nearest_pivot({ 3, 3 }, map);

    EXPECT_EQ(n, 0);
  }

  TEST(engine, eval_hierarchical_grid_nearest_pivot2) {
    using eval::hierarchical_grid::map_info;
    using eval::hierarchical_grid::nearest_pivot;
    using eval::hierarchical_grid::pivot;

    constexpr auto width  = 7;
    constexpr auto height = 7;
    constexpr auto size   = width * height;

    constexpr auto grid = array<int8_t, size> {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 1, 1, 1, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0
    };

    auto const map = map_info {
      .grid_size    = { width, height },
      .grid_stride  = width,
      .grid_scale   = 10,
      .grid_data    = grid,
      .block_size   = { 6, 6 },
      .block_stride = 2,
      .available    = [](int8_t const x) { return x <= 0; },
      .pivots      = { pivot { .block         = { 0, 0 },
                          .position      = { 3, 0 },
                          .is_horizontal = true },
                  pivot { .block       = { 0, 0 },
                          .position    = { 0, 3 },
                          .is_vertical = true } },
      .blocks      = { sl::vector<sl::index> { 0, 1 } }
    };

    auto const n = nearest_pivot({ 3, 3 }, map);

    EXPECT_EQ(n, 1);
  }
}
