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

#include "../../laplace/engine/eval/astar.impl.h"
#include <gtest/gtest.h>

namespace laplace::test {
  namespace astar = engine::eval::astar;

  using std::max, engine::intval, astar::link;

  TEST(engine, eval_astar_exists) {
    constexpr auto width  = 20;
    constexpr auto height = 10;
    constexpr auto size   = width * height;

    constexpr std::array<uint8_t, size> map = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, //
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    auto sight = [](const sl::index, const sl::index) {
      return false;
    };

    auto neighbours = [&](const sl::index node,
                          const sl::index n) -> link {
      if (node < 0 || node >= map.size())
        return {};

      if (map[node] == 1)
        return {};

      switch (n) {
        case 0: return { .node = node - 1, .distance = 1 };
        case 1: return { .node = node + 1, .distance = 1 };
        case 2: return { .node = node - width, .distance = 1 };
        case 3: return { .node = node + width, .distance = 1 };
      }

      return {};
    };

    auto heuristic = [&](const sl::index a,
                         const sl::index b) -> intval {
      const intval x0 = static_cast<intval>(a % width);
      const intval y0 = static_cast<intval>(a / width);

      const intval x1 = static_cast<intval>(b % width);
      const intval y1 = static_cast<intval>(b / width);

      return max(abs(x1 - x0), abs(y1 - y0));
    };

    auto node_index = [&](const sl::index x,
                          const sl::index y) -> sl::index {
      return y * width + x;
    };

    auto exists = [&](const sl::index source,
                      const sl::index destination) {
      auto state = astar::init<false, astar::_basic_node>(source,
                                                          destination);

      for (;;) {
        auto s = astar::loop(sight, neighbours, heuristic, state);

        if (s == astar::status::success) {
          return true;
        }

        if (s == astar::status::failed) {
          break;
        }
      }

      return false;
    };

    EXPECT_TRUE(exists(node_index(1, 1), node_index(11, 8)));
    EXPECT_FALSE(exists(node_index(1, 8), node_index(18, 1)));
  }

  TEST(engine, eval_astar_search) {
    constexpr auto width  = 10;
    constexpr auto height = 10;
    constexpr auto size   = width * height;

    constexpr std::array<uint8_t, size> map = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //
      1, 0, 0, 0, 0, 0, 0, 1, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 1, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 0, 0, 0, 0, 1, 0, 0, 0, 1, //
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    auto sight = [](const sl::index, const sl::index) {
      return false;
    };

    auto neighbours = [&](const sl::index node,
                          const sl::index n) -> link {
      if (node < 0 || node >= map.size())
        return {};

      if (map[node] == 1)
        return {};

      switch (n) {
        case 0: return { .node = node - 1, .distance = 10 };
        case 1: return { .node = node + 1, .distance = 10 };
        case 2: return { .node = node - width, .distance = 10 };
        case 3: return { .node = node + width, .distance = 10 };
      };

      return {};
    };

    auto heuristic = [&](const sl::index a,
                         const sl::index b) -> intval {
      const intval x0 = static_cast<intval>(a % width);
      const intval y0 = static_cast<intval>(a / width);

      const intval x1 = static_cast<intval>(b % width);
      const intval y1 = static_cast<intval>(b / width);

      return ((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) * 10;
    };

    auto node_index = [&](const sl::index x,
                          const sl::index y) -> sl::index {
      return y * width + x;
    };

    auto search =
        [&](const sl::index source,
            const sl::index destination) -> sl::vector<sl::index> {
      auto state = astar::init<false, astar::_basic_node>(source,
                                                          destination);

      for (;;) {
        auto s = astar::loop(sight, neighbours, heuristic, state);

        if (s == astar::status::success) {
          return astar::finish<astar::_basic_node>(
              state.closed, source, destination);
        }

        if (s == astar::status::failed) {
          break;
        }
      }

      return {};
    };

    auto path = search(node_index(1, 8), node_index(8, 1));

    auto path_map = map;

    for (auto n : path) { path_map[n] = 2; }

    auto s = std::string {};

    s.append(1, '\n');

    for (sl::index j = 0; j < height; j++) {
      s.append(1, ' ');

      for (sl::index i = 0; i < width; i++) {
        const auto c = path_map[j * width + i];

        s.append(1, ' ');

        if (c == 1) {
          s.append(1, 'x');
        } else if (c == 2) {
          s.append(1, 'o');
        } else {
          s.append(1, ' ');
        }
      }

      s.append(1, '\n');
    }

    s.append(1, '\n');

    std::cerr << s;

    EXPECT_FALSE(path.empty());
  }

  TEST(engine, eval_astar_thera) {
    constexpr auto width  = 6;
    constexpr auto height = 3;
    constexpr auto size   = width * height;

    constexpr std::array<uint8_t, size> map = { 1, 1, 1, 1, 1, 1, //
                                                1, 0, 0, 0, 0, 1, //
                                                1, 1, 1, 1, 1, 1 };

    auto neighbours = [&](const sl::index node,
                          const sl::index n) -> link {
      if (node < 0 || node >= map.size())
        return {};

      if (map[node] == 1)
        return {};

      switch (n) {
        case 0: return { .node = node - 1, .distance = 10 };
        case 1: return { .node = node + 1, .distance = 10 };
        case 2: return { .node = node - width, .distance = 10 };
        case 3: return { .node = node + width, .distance = 10 };
      };

      return {};
    };

    auto heuristic = [&](const sl::index a,
                         const sl::index b) -> intval {
      const intval x0 = static_cast<intval>(a % width);
      const intval y0 = static_cast<intval>(a / width);

      const intval x1 = static_cast<intval>(b % width);
      const intval y1 = static_cast<intval>(b / width);

      return ((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) * 10;
    };

    auto sight = [&](const sl::index a, const sl::index b) -> bool {
      const auto x0 = a % width;
      const auto y0 = a / width;

      const auto x1 = b % width;
      const auto y1 = b / width;

      return x0 == x1 || y0 == y1;
    };

    auto node_index = [&](const sl::index x,
                          const sl::index y) -> sl::index {
      return y * width + x;
    };

    auto search =
        [&](const sl::index source,
            const sl::index destination) -> sl::vector<sl::index> {
      auto state = astar::init<false, astar::_node_theta>(source,
                                                          destination);

      for (;;) {
        auto s = astar::loop(sight, neighbours, heuristic, state);

        if (s == astar::status::success) {
          return astar::finish<astar::_node_theta>(
              state.closed, source, destination);
        }

        if (s == astar::status::failed) {
          break;
        }
      }

      return {};
    };

    auto path = search(node_index(1, 1), node_index(4, 1));

    EXPECT_EQ(path.size(), 2u);
  }
}
