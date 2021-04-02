/*  test/unittests/ee_maze.test.cpp
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

#include "../../laplace/engine/eval/maze.h"
#include "../../laplace/engine/eval/random.h"
#include <gtest/gtest.h>

namespace laplace::test {
  namespace maze = engine::eval::maze;

  using std::array, std::random_device, std::cerr, std::string;

  TEST(engine, eval_maze) {
    constexpr auto width  = 25;
    constexpr auto height = 21;

    auto map = array<uint8_t, width * height> {};

    auto random = random_device {};

    maze::generate({ width, height }, map, [&random]() {
      return static_cast<uint64_t>(random());
    });

    auto s = string {};

    s.append(1, '\n');

    for (size_t j = 0; j < height; j++) {
      s.append(1, ' ');
      for (size_t i = 0; i < width; i++) {
        s.append(1, ' ');
        s.append(1, (map[j * width + i] == 0 ? ' ' : 'x'));
      }
      s.append(1, '\n');
    }

    s.append(1, '\n');

    cerr << s;
  }
}
