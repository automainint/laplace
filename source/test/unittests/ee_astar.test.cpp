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

#include "../../laplace/engine/eval/astar.h"
#include <gtest/gtest.h>

namespace laplace::test {
  namespace astar = engine::eval::astar;

  using std::max, engine::intval, astar::vlink, astar::link,
      astar::exists, astar::search;

  TEST(engine, eval_astar) {
    constexpr auto width  = 20;
    constexpr auto height = 10;
    constexpr auto size   = width * height;

    constexpr std::array<uint8_t, size> map = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    auto neighbours = [&](const size_t node) -> vlink {
      if (node >= map.size())
        return {};

      if (map[node] == 1)
        return {};

      return { link { .node = node - 1, .distance = 1 },
               link { .node = node + 1, .distance = 1 },
               link { .node = node - width, .distance = 1 },
               link { .node = node + width, .distance = 1 } };
    };

    auto heuristic = [&](const size_t a, const size_t b) -> intval {
      const intval x0 = static_cast<intval>(a % width);
      const intval y0 = static_cast<intval>(a / width);

      const intval x1 = static_cast<intval>(b % width);
      const intval y1 = static_cast<intval>(b / width);

      return max(abs(x1 - x0), abs(y1 - y0));
    };

    auto node_index = [&](const size_t x, const size_t y) -> size_t {
      return y * width + x;
    };

    EXPECT_TRUE(exists(neighbours, heuristic, node_index(1, 1), node_index(11, 8)));
    EXPECT_FALSE(exists(neighbours, heuristic, node_index(1, 8), node_index(18, 1)));
  }
}
