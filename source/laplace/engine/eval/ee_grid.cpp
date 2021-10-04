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

  void merge(vec2z const              size,
             span<int8_t> const       dst,
             span<int8_t const> const src,
             op const                 merge_op) noexcept {

    auto const count = size.x() * size.y();

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

  void merge(vec2z const              dst_size,
             span<int8_t> const       dst,
             vec2z const              src_size,
             vec2i const              src_offset,
             span<int8_t const> const src,
             op const                 merge_op) noexcept {

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

        auto const n = (src_offset.y() + y) * dst_size.x() +
                       (src_offset.x() + x);

        dst[n] = merge_op(dst[n], src[y * src_size.x() + x]);
      }
    }
  }

  [[nodiscard]] auto trace_line(vec2z const    a,
                                vec2z const    b,
                                fn_point const point) noexcept
      -> bool {

    auto const abs_delta = [](sl::index const x0,
                              sl::index const x1) {
      return x0 < x1 ? x1 - x0 : x0 - x1;
    };

    auto const dx = abs_delta(a.x(), b.x());
    auto const dy = abs_delta(a.y(), b.y());

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

  [[nodiscard]] auto neighbors4(sl::whole const          width,
                                sl::whole const          stride,
                                intval const             scale,
                                span<int8_t const> const map,
                                fn_available const       available,
                                sl::index const          position,
                                sl::index const n) noexcept -> link {

    if (position < 0 || position >= map.size() || stride <= 0) {
      return {};
    }

    auto const x = position % stride;
    auto const y = position / stride;

    if (x < 0 || y < 0 || x >= width) {
      return {};
    }

    if (!available(map[position])) {
      return {};
    }

    auto check = [&](sl::index x, sl::index y) -> link {
      if (x < 0 || x >= width || y < 0) {
        return { .node = link::skip };
      }

      auto const n = y * stride + x;
      if (n >= map.size() || !available(map[n])) {
        return { .node = link::skip };
      }

      return link { .node = n, .distance = scale };
    };

    switch (n) {
      case 0: return check(x, y - 1);
      case 1: return check(x, y + 1);
      case 2: return check(x - 1, y);
      case 3: return check(x + 1, y);
    }

    return {};
  }

  [[nodiscard]] auto neighbors8(sl::whole const          width,
                                sl::whole const          stride,
                                intval const             scale,
                                span<int8_t const> const map,
                                fn_available const       available,
                                sl::index const          position,
                                sl::index const n) noexcept -> link {

    if (position < 0 || position >= map.size() || stride <= 0) {
      return {};
    }

    auto const x = position % stride;
    auto const y = position / stride;

    if (x < 0 || y < 0 || x >= width) {
      return {};
    }

    if (!available(map[position])) {
      return {};
    }

    auto check = [&](sl::index x, sl::index y,
                     intval distance) -> link {
      if (x < 0 || x >= width || y < 0) {
        return { .node = link::skip };
      }

      auto const n = y * stride + x;
      if (n >= map.size() || !available(map[n])) {
        return { .node = link::skip };
      }

      return link { .node = n, .distance = distance };
    };

    switch (n) {
      case 0: return check(x, y - 1, scale);
      case 1: return check(x, y + 1, scale);
      case 2: return check(x - 1, y, scale);
      case 3: return check(x + 1, y, scale);
    }

    auto const d = scale > 1 ? eval::sqrt2(scale) : 1;

    switch (n) {
      case 4: return check(x - 1, y - 1, d);
      case 5: return check(x + 1, y - 1, d);
      case 6: return check(x - 1, y + 1, d);
      case 7: return check(x + 1, y + 1, d);
    }

    return {};
  }

  [[nodiscard]] auto manhattan(sl::whole const stride,
                               intval const    scale,
                               sl::index const source,
                               sl::index const destination) noexcept
      -> intval {

    if (stride <= 0) {
      return 0;
    }

    auto const x0 = source % stride;
    auto const y0 = source / stride;

    auto const x1 = destination % stride;
    auto const y1 = destination / stride;

    auto const dx = x0 < x1 ? x1 - x0 : x0 - x1;
    auto const dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return (dx + dy) * scale;
  }

  [[nodiscard]] auto diagonal(sl::whole const stride,
                              intval const    scale,
                              sl::index const a,
                              sl::index const b) noexcept -> intval {

    if (stride <= 0) {
      return 0;
    }

    auto const x0 = a % stride;
    auto const y0 = a / stride;

    auto const x1 = b % stride;
    auto const y1 = b / stride;

    auto const dx = x0 < x1 ? x1 - x0 : x0 - x1;
    auto const dy = y0 < y1 ? y1 - y0 : y0 - y1;

    if (dx < dy) {
      return eval::sqrt2(scale) * dx + (dy - dx) * scale;
    }

    return eval::sqrt2(scale) * dy + (dx - dy) * scale;
  }

  [[nodiscard]] auto euclidean(sl::whole const stride,
                               intval const    scale,
                               sl::index const a,
                               sl::index const b) noexcept -> intval {

    if (stride <= 0) {
      return 0;
    }

    auto const x0 = a % stride;
    auto const y0 = a / stride;

    auto const x1 = b % stride;
    auto const y1 = b / stride;

    auto const dx = x0 < x1 ? x1 - x0 : x0 - x1;
    auto const dy = y0 < y1 ? y1 - y0 : y0 - y1;

    return eval::sqrt((dx * dx + dy * dy) * scale, scale);
  }

  [[nodiscard]] auto path_exists(sl::whole const          width,
                                 sl::whole const          stride,
                                 span<int8_t const> const map,
                                 fn_available const       available,
                                 vec2z const              source,
                                 vec2z const destination) noexcept
      -> bool {

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

    auto const sight = [](sl::index const a, sl::index const b) {
      return false;
    };

    auto const heuristic = [&](sl::index const a,
                               sl::index const b) -> intval {
      return manhattan(stride, 1, a, b);
    };

    auto const neighbors = [&](sl::index const p,
                               sl::index const n) -> link {
      return neighbors8(width, stride, 1, map, available, p, n);
    };

    auto const index_of = [&](vec2z const p) {
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

  [[nodiscard]] auto path_exists(sl::whole const          width,
                                 span<int8_t const> const map,
                                 fn_available const       available,
                                 vec2z const              source,
                                 vec2z const destination) noexcept
      -> bool {

    return path_exists(width, width, map, available, source,
                       destination);
  }

  [[nodiscard]] auto path_exists(sl::whole const          width,
                                 vec2z const              min,
                                 vec2z const              max,
                                 span<int8_t const> const map,
                                 fn_available const       available,
                                 vec2z const              source,
                                 vec2z const destination) noexcept
      -> bool {

    return path_exists(
        max.x() - min.x(), width,
        span { map.begin() + min.y() * width + min.x(),
               map.begin() + (max.y() - 1) * width + max.x() },
        available, source - min, destination - min);
  }

  [[nodiscard]] auto path_search_init(
      vec2z const              size,
      vec2z const              origin,
      sl::whole const          stride,
      intval const             scale,
      span<int8_t const> const map,
      fn_available const       available,
      vec2z const              source,
      vec2z const              destination) noexcept -> _state {

    if (size.x() * size.y() > map.size()) {
      error_("Invalid map size.", __FUNCTION__);
      return {};
    }

    if (size.x() <= 0 || size.y() <= 0) {
      return {};
    }

    auto const width = size.x();

    auto s = _state {};

    auto const index_of = [&](vec2z const p) {
      return p.y() * stride + p.x();
    };

    s.astar = astar::init<true, astar::_node_theta>(
        index_of(source - origin), index_of(destination - origin));

    s.origin = origin;
    s.stride = stride;

    s.heuristic = [stride, scale](sl::index const a,
                                  sl::index const b) -> intval {
      return euclidean(stride, scale, a, b);
    };

    s.neighbors = [width, stride, scale, map, available](
                      sl::index const p, sl::index const n) -> link {
      return neighbors8(width, stride, scale, map, available, p, n);
    };

    s.sight = [size, stride, available,
               map](sl::index const a, sl::index const b) -> bool {
      auto const point_of = [&](const sl::index n) {
        return vec2z { n % stride, n / stride };
      };

      auto const p0 = point_of(a);
      auto const p1 = point_of(b);

      if (p0.x() < 0 || p0.x() >= size.x() || p0.y() < 0 ||
          p0.y() >= size.y() || p1.x() < 0 || p1.x() >= size.x() ||
          p1.y() < 0 || p1.y() >= size.y()) {
        return false;
      }

      return trace_line(p0, p1, [&](vec2z const p) {
        return available(map[p.y() * stride + p.x()]);
      });
    };

    return s;
  }

  auto path_search_init(vec2z const              size,
                        intval const             scale,
                        span<int8_t const> const map,
                        fn_available const       available,
                        vec2z const              source,
                        vec2z const destination) noexcept -> _state {

    return path_search_init(size, {}, size.x(), scale, map, available,
                            source, destination);
  }

  auto path_search_init(vec2z const              size,
                        vec2z const              min,
                        vec2z const              max,
                        intval const             scale,
                        span<int8_t const> const map,
                        fn_available const       available,
                        vec2z const              source,
                        vec2z const destination) noexcept -> _state {

    return path_search_init(
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

  [[nodiscard]] auto path_search_finish(_state const &state) noexcept
      -> sl::vector<vec2z> {
    if (state.stride <= 0) {
      return {};
    }

    auto const v = astar::finish<astar::_node_theta>(
        state.astar.closed, state.astar.source, state.astar.nearest);

    auto path = sl::vector<vec2z>(v.size());

    auto const point_of = [&](sl::index const n) {
      return state.origin +
             vec2z { n % state.stride, n / state.stride };
    };

    for (sl::index i = 0; i < v.size(); i++) {
      path[i] = point_of(v[i]);
    }

    return path;
  }

  void convolve(vec2z const              size,
                span<int8_t> const       dst,
                span<int8_t const> const src,
                vec2z const              fp_size,
                vec2z const              center,
                span<int8_t const> const footprint) noexcept {

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

        auto const i0 = i - center.x();
        auto const j0 = j - center.y();
        auto const i1 = i0 + fp_size.x();
        auto const j1 = j0 + fp_size.y();

        auto const x0 = max<sl::index>(0, i0) - i0;
        auto const y0 = max<sl::index>(0, j0) - j0;
        auto const x1 = min<sl::index>(size.x(), i1) - i0;
        auto const y1 = min<sl::index>(size.y(), j1) - j0;

        for (sl::index y = y0; y < y1; y++)
          for (sl::index x = x0; x < x1; x++) {
            if (footprint[y * fp_size.x() + x] <= 0) {
              continue;
            }

            dst[(j0 + y) * size.x() + i0 + x] = 1;
          }
      }
  }

  auto nearest(vec2z const                  position,
               vec2z const                  size,
               span<int8_t const> const     map,
               function<bool(int8_t)> const condition) noexcept
      -> vec2z {

    if (size.x() <= 0 || size.y() <= 0) {
      error_("Invalid size.", __FUNCTION__);
      return {};
    }

    if (!condition) {
      error_("Invalid condition.", __FUNCTION__);
      return {};
    }

    auto const x0 = max<sl::index>(0,
                                   min(position.x(), size.x() - 1));
    auto const y0 = max<sl::index>(0,
                                   min(position.y(), size.y() - 1));

    if (map[y0 * size.x() + x0] <= 0) {
      return { x0, y0 };
    }

    auto x        = x0;
    auto y        = y0;
    auto distance = sl::index { -1 };

    auto do_point = [&](sl::index const i, sl::index const j) {
      if (!condition(map[j * size.x() + i])) {
        return;
      }

      auto const s = math::square_length(vec2z { i, j } -
                                         vec2z { x0, y0 });

      if (distance < 0 || s < distance) {
        x = i;
        y = j;

        distance = s;
      }
    };

    auto const max_radius = max(max(x0, size.x() - x0),
                                max(y0, size.y() - y0));

    for (sl::index radius = 1; radius <= max_radius; radius++) {
      auto const i0 = max<sl::index>(0, x0 - radius);
      auto const j0 = max<sl::index>(0, y0 - radius);
      auto const i1 = min<sl::index>(x0 + radius, size.x() - 1);
      auto const j1 = min<sl::index>(y0 + radius, size.y() - 1);

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
