/*  laplace/engine/eval/ee_maze.cpp
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

#include "maze.h"

#include "grid.h"

namespace laplace::engine::eval::maze {
  using std::span, std::function, std::tuple, std::tie, std::min;

  void generate(vec2z const size, span<int8_t> const map,
                fn_random const random, log_handler log) noexcept {

    if (size.x() < 0 || size.y() < 0) {
      log(log_event::error, "Invalid size value.", __FUNCTION__);
      return;
    }

    if (map.size() < size.x() * size.y()) {
      log(log_event::error, "Invalid map size.", __FUNCTION__);
      return;
    }

    auto const index_of = [width = size.x()](vec2z const p) {
      return p.y() * width + p.x();
    };

    for (sl::index i = 0, i1 = size.x() * size.y(); i < i1; i++) {
      map[i] = walkable;
    }

    auto const cols = (size.x() - 1) / 2;
    auto const rows = (size.y() - 1) / 2;

    auto const i1 = cols * 2 + 1;
    auto const j1 = rows * 2 + 1;

    for (sl::index col = 0; col <= cols; col++) {
      for (sl::index j = 0; j < j1; j++) {
        map[index_of({ col * 2, j })] = wall;
      }
    }

    for (sl::index row = 0; row <= rows; row++) {
      for (sl::index i = 1; i < i1; i += 2) {
        map[index_of({ i, row * 2 })] = wall;
      }
    }

    auto const gen_caves =
        [](sl::index const cols,
           sl::index const rows) -> sl::vector<vec2z> {
      auto caves = sl::vector<vec2z>(cols * rows);

      for (sl::index i = 0; i < cols; i++) {
        for (sl::index j = 0; j < rows; j++)
          caves[j * cols + i] = vec2z { 1 + i * 2, 1 + j * 2 };
      }

      caves.resize(caves.size() * 2);

      for (sl::index i = 0, i0 = caves.size() / 2; i < i0; i++) {
        caves[i0 + i] = caves[i];
      }

      return caves;
    };

    auto const shuffle =
        [&random](sl::vector<vec2z> caves) -> sl::vector<vec2z> {
      auto v = sl::vector<vec2z> {};
      v.reserve(caves.size());

      while (!caves.empty()) {
        auto const n = random() % caves.size();
        v.emplace_back(caves[n]);
        caves.erase(caves.begin() + n);
      }

      return v;
    };

    auto caves = shuffle(gen_caves(cols, rows));

    auto const limit = caves.size() / 2 + 1;

    while (caves.size() > limit) {
      auto const step =
          [&size](vec2z const     a,
                  sl::index const dir) -> tuple<vec2z, vec2z> {
        if (dir == 0) {
          if (a.x() <= 2)
            return { { a.x() + 2, a.y() }, { a.x() + 1, a.y() } };
          return { { a.x() - 2, a.y() }, { a.x() - 1, a.y() } };
        }

        if (dir == 1) {
          if (a.y() <= 2)
            return { { a.x(), a.y() + 2 }, { a.x(), a.y() + 1 } };
          return { { a.x(), a.y() - 2 }, { a.x(), a.y() - 1 } };
        }

        if (dir == 2) {
          if (a.x() + 3 >= size.x())
            return { { a.x() - 2, a.y() }, { a.x() - 1, a.y() } };
          return { { a.x() + 2, a.y() }, { a.x() + 1, a.y() } };
        }

        if (a.y() + 3 >= size.y())
          return { { a.x(), a.y() - 2 }, { a.x(), a.y() - 1 } };
        return { { a.x(), a.y() + 2 }, { a.x(), a.y() + 1 } };
      };

      auto dir  = static_cast<sl::index>(random() % 4);
      auto cave = static_cast<sl::index>(random() % caves.size());

      vec2z a     = caves[cave];
      auto [b, m] = step(a, dir);

      auto const cave0 = cave;
      auto const dir0  = dir;

      while (grid::path_exists(
          a, b, [](int8_t const state) { return state == walkable; },
          grid::area_of(size, map))) {

        cave = (cave + 1) % caves.size();
        a    = caves[cave];

        if (cave == cave0) {
          dir = (dir + 1) % 4;

          if (dir == dir0)
            break;
        }

        tie(b, m) = step(a, dir);
      }

      caves.erase(caves.begin() + cave);

      map[index_of(m)] = walkable;
    }
  }

  void stretch(vec2z const dst_size, span<int8_t> const dst,
               vec2z const src_size, span<const int8_t> const src,
               sl::index const tunnel_size, sl::index const gate_size,
               log_handler log) noexcept {

    if (dst_size.x() < 0 || dst_size.y() < 0 || src_size.x() < 0 ||
        src_size.y() < 0) {
      log(log_event::error, "Invalid size value.", __FUNCTION__);
      return;
    }

    if (dst.size() < dst_size.x() * dst_size.y()) {
      log(log_event::error, "Invalid destination size.",
          __FUNCTION__);
      return;
    }

    if (src.size() < src_size.x() * src_size.y()) {
      log(log_event::error, "Invalid source size.", __FUNCTION__);
      return;
    }

    for (sl::index i = 0, i1 = dst_size.x() * dst_size.y(); i < i1;
         i++)
      dst[i] = walkable;
    
    for (sl::index j = 0; j < src_size.y(); j++) {
      for (sl::index i = 0; i < src_size.x(); i++) {

        if (src[j * src_size.x() + i] == walkable)
          continue;

        auto x0 = (i * dst_size.x() + src_size.x() / 2) /
                  src_size.x();
        auto y0 = (j * dst_size.y() + src_size.y() / 2) /
                  src_size.y();

        auto x1 = ((i + 1) * dst_size.x() + src_size.x() / 2) /
                  src_size.x();
        auto y1 = ((j + 1) * dst_size.y() + src_size.y() / 2) /
                  src_size.y();

        if (x1 == x0)
          x1 = x0 + 1;

        if (y1 == y0)
          y1 = y0 + 1;

        auto const wx0 = i > 0 &&
                         src[j * src_size.x() + i - 1] == walkable;
        auto const wx1 = i + 1 < src_size.x() &&
                         src[j * src_size.x() + i + 1] == walkable;
        auto const wy0 = j > 0 &&
                         src[(j - 1) * src_size.x() + i] == walkable;
        auto const wy1 = j + 1 < src_size.y() &&
                         src[(j + 1) * src_size.x() + i] == walkable;

        if (wx0) {
          if (wy0 || wy1)
            x0 = min(x0 + gate_size, dst_size.x());
          else
            x0 = min(x0 + tunnel_size, dst_size.x());
        }

        if (wx1) {
          if (wy0 || wy1)
            x1 -= min(x1, gate_size);
          else
            x1 -= min(x1, tunnel_size);
        }

        if (wy0) {
          if (wx0 || wx1)
            y0 = min(y0 + gate_size, dst_size.y());
          else
            y0 = min(y0 + tunnel_size, dst_size.y());
        }

        if (wy1) {
          if (wx0 || wx1)
            y1 -= min(y1, gate_size);
          else
            y1 -= min(y1, tunnel_size);
        }

        for (sl::index y = y0; y < y1; y++) {
          for (sl::index x = x0; x < x1; x++) {
            auto const n = y * dst_size.x() + x;

            if (n < dst.size())
              dst[y * dst_size.x() + x] = wall;
          }
        }
      }
    }
  }
}
