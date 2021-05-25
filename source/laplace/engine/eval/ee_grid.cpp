/*  laplace/engine/eval/ee_grid.cpp
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

#include "grid.h"
#include "integral.h"

namespace laplace::engine::eval::grid {
  using std::span, std::min, std::function, astar::vlink;

  void merge(const vec2z size, const span_byte dst,
             const span_cbyte src, const op merge_op) noexcept {

    const auto count = size.x() * size.y();

    if (count > dst.size()) {
      error_("Invalid destination size.", __FUNCTION__);
      return;
    }

    if (count > src.size()) {
      error_("Invalid source size.", __FUNCTION__);
      return;
    }

    for (sl::index i = 0; i < count; i++) {
      dst[i] = merge_op(dst[i], src[i]);
    }
  }

  void merge(const vec2z dst_size, const span_byte dst,
             const vec2z src_size, const vec2i src_offset,
             const span_cbyte src, const op merge_op) noexcept {

    if (dst_size.x() * dst_size.y() > dst.size()) {
      error_("Invalid destination size.", __FUNCTION__);
      return;
    }

    if (src_size.x() * src_size.y() > src.size()) {
      error_("Invalid source size.", __FUNCTION__);
      return;
    }

    for (sl::index y = 0; y < src_size.y(); y++) {
      for (sl::index x = 0; x < src_size.x(); x++) {

        const auto n = (src_offset.y() + y) * dst_size.x() +
                       (src_offset.x() + x);

        dst[n] = merge_op(dst[n], src[y * src_size.x() + x]);
      }
    }
  }

  auto trace_line(const vec2z size, const vec2z a, const vec2z b,
                  const fn_point point) -> bool {

    const auto abs_delta = [](const sl::index x0, const sl::index x1) {
      return x0 < x1 ? x1 - x0 : x0 - x1;
    };

    const auto dx = abs_delta(a.x(), b.x());
    const auto dy = abs_delta(a.y(), b.y());

    if (dx > dy) {
      if (b.x() < a.x()) {
        return trace_line(size, b, a, point);
      }

      auto y = a.y();
      auto d = dx / 2;

      if (a.y() < b.y()) {
        for (auto x = a.x(); x <= b.x(); x++) {

          if (!point({ x, y })) {
            return false;
          }

          d += dy;

          if (d >= dx) {
            y++;
            d -= dx;
          }
        }
      } else {
        for (auto x = a.x(); x <= b.x(); x++) {

          if (!point({ x, y })) {
            return false;
          }

          d += dy;

          if (d >= dx) {
            y--;
            d -= dx;
          }
        }
      }
    } else {
      if (b.y() < a.y()) {
        return trace_line(size, b, a, point);
      }

      auto x = a.x();
      auto d = dy / 2;

      if (a.x() < b.x()) {
        for (auto y = a.y(); y <= b.y(); y++) {

          if (!point({ x, y })) {
            return false;
          }

          d += dx;

          if (d >= dy) {
            x++;
            d -= dy;
          }
        }
      } else {
        for (auto y = a.y(); y <= b.y(); y++) {

          if (!point({ x, y })) {
            return false;
          }

          d += dx;

          if (d >= dy) {
            x--;
            d -= dy;
          }
        }
      }
    }

    return true;
  }

  auto neighbors4(const sl::index width, const intval scale,
                  const span_cbyte map, const fn_available available,
                  const sl::index p) -> vlink {

    if (p < 0 || p >= map.size())
      return {};

    const auto x = p % width;
    const auto y = p / width;

    if (x <= 0 || y <= 0 || x >= width - 1)
      return {};

    if (!available(map[p]))
      return {};

    return { { .node = p - width, .distance = scale },
             { .node = p + width, .distance = scale },
             { .node = p - 1, .distance = scale },
             { .node = p + 1, .distance = scale } };
  }

  auto neighbors8(const sl::index width, const intval scale,
                  const span_cbyte map, const fn_available available,
                  const sl::index p) -> vlink {

    if (p < 0 || p >= map.size())
      return {};

    const auto x = p % width;
    const auto y = p / width;

    if (x <= 0 || y <= 0 || x >= width - 1)
      return {};

    if (!available(map[p]))
      return {};

    const auto d = scale > 1 ? eval::sqrt2(scale) : 1;

    return { { .node = p - width, .distance = scale },
             { .node = p + width, .distance = scale },
             { .node = p - 1, .distance = scale },
             { .node = p + 1, .distance = scale },
             { .node = p - width - 1, .distance = d },
             { .node = p - width + 1, .distance = d },
             { .node = p + width - 1, .distance = d },
             { .node = p + width + 1, .distance = d } };
  }

  auto manhattan(const sl::index width, const intval scale,
                 const sl::index a, const sl::index b) -> intval {

    if (width <= 0)
      return 0;

    const auto x0 = a % width;
    const auto y0 = a / width;

    const auto x1 = b % width;
    const auto y1 = b / width;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return (dx + dy) * scale;
  }

  auto diagonal(const sl::index width, const intval scale,
                const sl::index a, const sl::index b) -> intval {

    if (width <= 0)
      return 0;

    const auto x0 = a % width;
    const auto y0 = a / width;

    const auto x1 = b % width;
    const auto y1 = b / width;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    if (dx < dy) {
      return eval::sqrt2(scale) * dx + (dy - dx) * scale;
    }

    return eval::sqrt2(scale) * dy + (dx - dy) * scale;
  }

  auto euclidean(const sl::index width, const intval scale,
                 const sl::index a, const sl::index b) -> intval {

    if (width <= 0)
      return 0;

    const auto x0 = a % width;
    const auto y0 = a / width;

    const auto x1 = b % width;
    const auto y1 = b / width;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return eval::sqrt((dx * dx + dy * dy) * scale, scale);
  }

  auto path_exists(const sl::index width, const span_cbyte map,
                   const fn_available available, const vec2z a,
                   const vec2z b) -> bool {

    if (width <= 0) {
      return false;
    }

    const auto heuristic = [width](const sl::index a,
                                   const sl::index b) -> intval {
      return manhattan(width, 1, a, b);
    };

    const auto neighbors = [width, &map,
                            &available](const sl::index p) -> vlink {
      return neighbors8(width, 1, map, available, p);
    };

    const auto index_of = [width](const vec2z p) {
      return p.y() * width + p.x();
    };

    return astar::exists(
        neighbors, heuristic, index_of(a), index_of(b));
  }

  auto path_search(const vec2z size, const intval scale,
                   const span_cbyte map, const fn_available available,
                   const vec2z a, const vec2z b) -> sl::vector<vec2z> {

    if (size.x() * size.y() > map.size()) {
      error_("Invalid map size.", __FUNCTION__);
      return {};
    }

    if (size.x() <= 0 || size.y() <= 0) {
      return {};
    }

    const auto heuristic = [width = size.x(), scale](
                               const sl::index a,
                               const sl::index b) -> intval {
      return euclidean(width, scale, a, b);
    };

    const auto neighbors = [width = size.x(), scale, &map,
                            &available](const sl::index p) -> vlink {
      return neighbors8(width, scale, map, available, p);
    };

    const auto point_of = [width = size.x()](const sl::index n) {
      return vec2z { n % width, n / width };
    };

    const auto sight = [&size, &map, &available, &point_of](
                           const sl::index a,
                           const sl::index b) -> bool {
      return trace_line(
          size, point_of(a), point_of(b), [&](const vec2z p) {
            return available(map[p.y() * size.x() + p.x()]);
          });
    };

    const auto index_of = [width = size.x()](const vec2z p) {
      return p.y() * width + p.x();
    };

    const auto v = astar::search_theta(
        neighbors, heuristic, sight, index_of(a), index_of(b));

    auto path = sl::vector<vec2z> {};

    if (v.size() == 1) {
      path.emplace_back(point_of(v[0]));

    } else if (v.size() > 1) {
      const auto draw_line = [&path, &size](
                                 const vec2z a, const vec2z b) {
        trace_line(size, a, b, [&path](const vec2z p) {
          if (path.empty() || path.back() != p) {
            path.emplace_back(p);
          }
          return true;
        });
      };

      for (sl::index i = 1; i < v.size(); i++) {
        draw_line(point_of(v[i - 1]), point_of(v[i]));
      }
    }

    return path;
  }
}
