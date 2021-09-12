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

#include "astar.impl.h"
#include "integral.h"

namespace laplace::engine::eval::grid {
  using std::span, std::min, std::max, std::function, astar::link;

  void merge(const vec2z              size,
             const span<int8_t>       dst,
             const span<const int8_t> src,
             const op                 merge_op) noexcept {

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

  void merge(const vec2z              dst_size,
             const span<int8_t>       dst,
             const vec2z              src_size,
             const vec2i              src_offset,
             const span<const int8_t> src,
             const op                 merge_op) noexcept {

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

  [[nodiscard]] auto trace_line(const vec2z    a,
                                const vec2z    b,
                                const fn_point point) noexcept
      -> bool {

    const auto abs_delta = [](const sl::index x0,
                              const sl::index x1) {
      return x0 < x1 ? x1 - x0 : x0 - x1;
    };

    const auto dx = abs_delta(a.x(), b.x());
    const auto dy = abs_delta(a.y(), b.y());

    if (dx > dy) {
      if (b.x() < a.x()) {
        return trace_line(b, a, point);
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
        return trace_line(b, a, point);
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

  [[nodiscard]] auto neighbors4(const sl::whole          width,
                                const sl::whole          stride,
                                const intval             scale,
                                const span<const int8_t> map,
                                const fn_available       available,
                                const sl::index          position,
                                const sl::index n) noexcept -> link {

    if (position < 0 || position >= map.size() || stride <= 0) {
      return {};
    }

    const auto x = position % stride;
    const auto y = position / stride;

    if (x < 0 || y < 0 || x >= width) {
      return {};
    }

    if (!available(map[position])) {
      return {};
    }

    auto check = [&](sl::index p) -> link {
      if (p < 0 || p >= map.size() || p % stride >= width ||
          !available(map[p])) {
        return { .node = link::skip };
      }

      return link { .node = p, .distance = scale };
    };

    switch (n) {
      case 0: return check(position - stride);
      case 1: return check(position + stride);
      case 2: return check(position - 1);
      case 3: return check(position + 1);
    }

    return {};
  }

  [[nodiscard]] auto neighbors8(const sl::whole          width,
                                const sl::whole          stride,
                                const intval             scale,
                                const span<const int8_t> map,
                                const fn_available       available,
                                const sl::index          position,
                                const sl::index n) noexcept -> link {

    if (position < 0 || position >= map.size() || stride <= 0) {
      return {};
    }

    const auto x = position % stride;
    const auto y = position / stride;

    if (x < 0 || y < 0 || x >= width) {
      return {};
    }

    if (!available(map[position])) {
      return {};
    }

    auto check = [&](sl::index p, intval distance) -> link {
      if (p < 0 || p >= map.size() || p % stride >= width ||
          !available(map[p])) {
        return { .node = link::skip };
      }

      return link { .node = p, .distance = distance };
    };

    switch (n) {
      case 0: return check(position - stride, scale);
      case 1: return check(position + stride, scale);
      case 2: return check(position - 1, scale);
      case 3: return check(position + 1, scale);
    }

    const auto d = scale > 1 ? eval::sqrt2(scale) : 1;

    switch (n) {
      case 4: return check(position - stride - 1, d);
      case 5: return check(position - stride + 1, d);
      case 6: return check(position + stride - 1, d);
      case 7: return check(position + stride + 1, d);
    }

    return {};
  }

  [[nodiscard]] auto manhattan(const sl::whole stride,
                               const intval    scale,
                               const sl::index source,
                               const sl::index destination) noexcept
      -> intval {

    if (stride <= 0) {
      return 0;
    }

    const auto x0 = source % stride;
    const auto y0 = source / stride;

    const auto x1 = destination % stride;
    const auto y1 = destination / stride;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return (dx + dy) * scale;
  }

  [[nodiscard]] auto diagonal(const sl::whole stride,
                              const intval    scale,
                              const sl::index a,
                              const sl::index b) noexcept -> intval {

    if (stride <= 0) {
      return 0;
    }

    const auto x0 = a % stride;
    const auto y0 = a / stride;

    const auto x1 = b % stride;
    const auto y1 = b / stride;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    if (dx < dy) {
      return eval::sqrt2(scale) * dx + (dy - dx) * scale;
    }

    return eval::sqrt2(scale) * dy + (dx - dy) * scale;
  }

  [[nodiscard]] auto euclidean(const sl::whole stride,
                               const intval    scale,
                               const sl::index a,
                               const sl::index b) noexcept -> intval {

    if (stride <= 0) {
      return 0;
    }

    const auto x0 = a % stride;
    const auto y0 = a / stride;

    const auto x1 = b % stride;
    const auto y1 = b / stride;

    const auto dx = x0 < x1 ? x1 - x0 : x0 - x1;
    const auto dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return eval::sqrt((dx * dx + dy * dy) * scale, scale);
  }

  [[nodiscard]] auto _path_exists_basic(
      const sl::whole          width,
      const sl::whole          stride,
      const span<const int8_t> map,
      const fn_available       available,
      const vec2z              source,
      const vec2z              destination) noexcept -> bool {

    if (stride <= 0) {
      return false;
    }

    if (source.x() < 0 || source.x() >= width || source.y() < 0 ||
        destination.x() < 0 || destination.x() >= width ||
        destination.y() < 0) {
      return false;
    }

    if (source.y() * stride + source.x() >= map.size() ||
        destination.y() * stride + destination.x() >= map.size()) {
      return false;
    }

    if (trace_line(source, destination, [&](const vec2z p) {
          return available(map[p.y() * stride + p.x()]);
        })) {
      return true;
    }

    const auto sight = [](const sl::index a, const sl::index b) {
      return false;
    };

    const auto heuristic = [&](const sl::index a,
                               const sl::index b) -> intval {
      return manhattan(stride, 1, a, b);
    };

    const auto neighbors = [&](const sl::index p,
                               const sl::index n) -> link {
      return neighbors8(width, stride, 1, map, available, p, n);
    };

    const auto index_of = [&](const vec2z p) {
      return p.y() * stride + p.x();
    };

    auto state = astar::init<false, astar::_basic_node>(
        index_of(source), index_of(destination));

    for (;;) {
      auto result = astar::loop(sight, neighbors, heuristic, state);

      if (result == astar::status::success) {
        return true;
      }

      if (result == astar::status::failed) {
        break;
      }
    }

    return false;
  }

  [[nodiscard]] auto path_exists(const sl::whole          width,
                                 const span<const int8_t> map,
                                 const fn_available       available,
                                 const vec2z              source,
                                 const vec2z destination) noexcept
      -> bool {

    return _path_exists_basic(width, width, map, available, source,
                              destination);
  }

  [[nodiscard]] auto path_exists(const sl::whole          width,
                                 const vec2z              min,
                                 const vec2z              max,
                                 const span<const int8_t> map,
                                 const fn_available       available,
                                 const vec2z              source,
                                 const vec2z destination) noexcept
      -> bool {

    return _path_exists_basic(
        max.x() - min.x(), width,
        span { map.begin() + min.y() * width + min.x(),
               map.begin() + (max.y() - 1) * width + max.x() },
        available, source - min, destination - min);
  }

  [[nodiscard]] auto _path_search_init_basic(
      const vec2z              size,
      const vec2z              origin,
      const sl::whole          stride,
      const intval             scale,
      const span<const int8_t> map,
      const fn_available       available,
      const vec2z              source,
      const vec2z              destination) noexcept -> _state {

    if (size.x() * size.y() > map.size()) {
      error_("Invalid map size.", __FUNCTION__);
      return {};
    }

    if (size.x() <= 0 || size.y() <= 0) {
      return {};
    }

    const auto width = size.x();

    auto s = _state {};

    const auto index_of = [&](const vec2z p) {
      return p.y() * width + p.x();
    };

    s.astar = astar::init<true, astar::_node_theta>(
        index_of(source - origin), index_of(destination - origin));

    s.origin = origin;
    s.stride = stride;

    s.heuristic = [stride, scale](const sl::index a,
                                  const sl::index b) -> intval {
      return euclidean(stride, scale, a, b);
    };

    s.neighbors = [width, stride, scale, map, available](
                      const sl::index p, const sl::index n) -> link {
      return neighbors8(width, stride, scale, map, available, p, n);
    };

    s.sight = [size, stride, available,
               map](const sl::index a, const sl::index b) -> bool {
      const auto point_of = [&](const sl::index n) {
        return vec2z { n % stride, n / stride };
      };

      const auto p0 = point_of(a);
      const auto p1 = point_of(b);

      if (p0.x() < 0 || p0.x() >= size.x() || p0.y() < 0 ||
          p0.y() >= size.y() || p1.x() < 0 || p1.x() >= size.x() ||
          p1.y() < 0 || p1.y() >= size.y()) {
        return false;
      }

      return trace_line(p0, p1, [&](const vec2z p) {
        return available(map[p.y() * stride + p.x()]);
      });
    };

    return s;
  }

  auto path_search_init(const vec2z              size,
                        const intval             scale,
                        const span<const int8_t> map,
                        const fn_available       available,
                        const vec2z              source,
                        const vec2z destination) noexcept -> _state {

    return _path_search_init_basic(size, {}, size.x(), scale, map,
                                   available, source, destination);
  }

  auto path_search_init(const vec2z              size,
                        const vec2z              min,
                        const vec2z              max,
                        const intval             scale,
                        const span<const int8_t> map,
                        const fn_available       available,
                        const vec2z              source,
                        const vec2z destination) noexcept -> _state {

    return _path_search_init_basic(
        max - min, min, size.x(), scale,
        span { map.begin() + min.y() * size.x() + min.x(),
               map.begin() + (max.y() - 1) * size.x() + max.x() },
        available, source, destination);
  }

  [[nodiscard]] auto path_search_loop(_state &state) noexcept
      -> astar::status {

    return astar::loop(state.sight, state.neighbors, state.heuristic,
                       state.astar);
  }

  [[nodiscard]] auto path_search_finish(const _state &state) noexcept
      -> sl::vector<vec2z> {
    if (state.stride <= 0) {
      return {};
    }

    const auto v = astar::finish<astar::_node_theta>(
        state.astar.closed, state.astar.source, state.astar.nearest);

    auto path = sl::vector<vec2z>(v.size());

    const auto point_of = [&](const sl::index n) {
      return state.origin +
             vec2z { n % state.stride, n / state.stride };
    };

    for (sl::index i = 0; i < v.size(); i++) {
      path[i] = point_of(v[i]);
    }

    return path;
  }

  void convolve(const vec2z        size,
                span<int8_t>       dst,
                span<const int8_t> src,
                const vec2z        fp_size,
                const vec2z        center,
                span<const int8_t> footprint) noexcept {

    if (size.x() < 0 || size.y() < 0) {
      error_("Invalid map size.", __FUNCTION__);
      return;
    }

    if (fp_size.x() < 0 || fp_size.y() < 0) {
      error_("Invalid footprint size.", __FUNCTION__);
      return;
    }

    if (src.size() != size.x() * size.y()) {
      error_("Invalid source.", __FUNCTION__);
      return;
    }

    if (dst.size() != size.x() * size.y()) {
      error_("Invalid destination.", __FUNCTION__);
      return;
    }

    if (footprint.size() != fp_size.x() * fp_size.y()) {
      error_("Invalid footprint.", __FUNCTION__);
      return;
    }

    for (sl::index j = 0; j < size.y(); j++)
      for (sl::index i = 0; i < size.x(); i++) {
        if (src[j * size.x() + i] <= 0) {
          continue;
        }

        const auto i0 = i - center.x();
        const auto j0 = j - center.y();
        const auto i1 = i0 + fp_size.x();
        const auto j1 = j0 + fp_size.y();

        const auto x0 = max<sl::index>(0, i0) - i0;
        const auto y0 = max<sl::index>(0, j0) - j0;
        const auto x1 = min<sl::index>(size.x(), i1) - i0;
        const auto y1 = min<sl::index>(size.y(), j1) - j0;

        for (sl::index y = y0; y < y1; y++)
          for (sl::index x = x0; x < x1; x++) {
            if (footprint[y * fp_size.x() + x] <= 0) {
              continue;
            }

            dst[(j0 + y) * size.x() + i0 + x] = 1;
          }
      }
  }

  auto nearest(const vec2z                  position,
               const vec2z                  size,
               span<const int8_t>           map,
               function<bool(const int8_t)> condition) noexcept
      -> vec2z {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return {};
    }

    if (!condition) {
      error_("Invalid condition.", __FUNCTION__);
      return {};
    }

    const auto x0 = max<sl::index>(0,
                                   min(position.x(), size.x() - 1));
    const auto y0 = max<sl::index>(0,
                                   min(position.y(), size.y() - 1));

    if (map[y0 * size.x() + x0] <= 0) {
      return { x0, y0 };
    }

    auto x        = x0;
    auto y        = y0;
    auto distance = sl::index { -1 };

    auto do_point = [&](const sl::index i, const sl::index j) {
      if (!condition(map[j * size.x() + i])) {
        return;
      }

      const auto s = math::square_length(vec2z { i, j } -
                                         vec2z { x0, y0 });

      if (distance < 0 || s < distance) {
        x = i;
        y = j;

        distance = s;
      }
    };

    const auto max_radius = max(max(x0, size.x() - x0),
                                max(y0, size.y() - y0));

    for (sl::index radius = 1; radius <= max_radius; radius++) {
      const auto i0 = max<sl::index>(0, x0 - radius);
      const auto j0 = max<sl::index>(0, y0 - radius);
      const auto i1 = min<sl::index>(x0 + radius, size.x() - 1);
      const auto j1 = min<sl::index>(y0 + radius, size.y() - 1);

      if (x0 - radius >= 0)
        for (sl::index j = j0 + 1; j < j1; j++) { do_point(i0, j); }

      if (x0 + radius < size.x())
        for (sl::index j = j0 + 1; j < j1; j++) { do_point(i1, j); }

      if (y0 - radius >= 0)
        for (sl::index i = i0; i <= i1; i++) { do_point(i, j0); }

      if (y0 + radius < size.y())
        for (sl::index i = i0; i <= i1; i++) { do_point(i, j1); }

      if (0 < distance && distance <= radius * radius) {
        break;
      }
    }

    return { x, y };
  }
}
