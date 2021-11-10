/*  Copyright (c) 2021 Mitya Selivanov
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

  void merge(vec2z const size, span<int8_t> const dst,
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

  void merge(vec2z const dst_size, span<int8_t> const dst,
             vec2z const src_size, vec2i const src_offset,
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

  auto trace_line(vec2z const a, vec2z const b,
                  fn_point const &point) noexcept -> bool {

    auto const abs_delta = [](sl::index const x0,
                              sl::index const x1) {
      return x0 < x1 ? x1 - x0 : x0 - x1;
    };

    auto const dx = abs_delta(a.x(), b.x());
    auto const dy = abs_delta(a.y(), b.y());

    if (dx > dy) {
      if (b.x() < a.x())
        return trace_line(b, a, point);

      auto y = a.y();
      auto d = dx / 2;

      if (a.y() < b.y()) {
        for (auto x = a.x(); x <= b.x(); x++) {

          if (!point({ x, y }))
            return false;

          d += dy;

          if (d >= dx) {
            y++;
            d -= dx;
          }
        }
      } else {
        for (auto x = a.x(); x <= b.x(); x++) {

          if (!point({ x, y }))
            return false;

          d += dy;

          if (d >= dx) {
            y--;
            d -= dx;
          }
        }
      }
    } else {
      if (b.y() < a.y())
        return trace_line(b, a, point);

      auto x = a.x();
      auto d = dy / 2;

      if (a.x() < b.x()) {
        for (auto y = a.y(); y <= b.y(); y++) {

          if (!point({ x, y }))
            return false;

          d += dx;

          if (d >= dy) {
            x++;
            d -= dy;
          }
        }
      } else {
        for (auto y = a.y(); y <= b.y(); y++) {

          if (!point({ x, y }))
            return false;

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

  auto area_of(vec2z const                   size,
               std::span<int8_t const> const map) noexcept
      -> rect_area {
    if constexpr (!_unsafe) {
      if (size.x() * size.y() < map.size()) {
        error_("Invalid size.", __FUNCTION__);
        return {};
      }
    }

    return rect_area { .map    = map,
                       .stride = size.x(),
                       .width  = size.x() };
  }

  auto submap(vec2z const min, vec2z const max,
              rect_area const area) noexcept -> rect_area {
    if (max.x() <= min.x() || max.y() <= min.y())
      return {};

    auto const a = min - area.origin;
    auto const b = max - area.origin;

    auto const i = a.y() * area.stride + a.x();
    auto const j = (b.y() - 1) * area.stride + b.x();

    if constexpr (!_unsafe) {
      if (i < 0 || i > area.map.size() || j < 0 ||
          j > area.map.size() || j < i) {
        error_("Invalid bounds.", __FUNCTION__);
        return {};
      }
    }

    return rect_area { .map    = span { area.map.begin() + i,
                                     area.map.begin() + j },
                       .stride = area.stride,
                       .width  = max.x() - min.x(),
                       .origin = min };
  }

  auto index_of(rect_area const area, vec2z const position,
                sl::index const invalid) noexcept -> sl::index {
    auto const p = position - area.origin;

    if (p.x() < 0 || p.y() < 0 || p.x() >= area.width)
      return invalid;

    auto const n = p.y() * area.stride + p.x();

    if (n >= area.map.size())
      return invalid;

    return n;
  }

  auto point_of(sl::whole const stride, sl::index const n) noexcept
      -> vec2z {
    if (stride <= 0)
      return {};

    return vec2z { n % stride, n / stride };
  }

  auto point_of(rect_area const area, sl::index const n) noexcept
      -> vec2z {
    if (area.stride <= 0)
      return {};

    return area.origin + vec2z { n % area.stride, n / area.stride };
  }

  auto contains(rect_area const area, vec2z const position) noexcept
      -> bool {
    return index_of(area, position) >= 0;
  }

  auto value(rect_area const area, sl::index const n,
             int8_t const invalid) noexcept -> int8_t {
    if (n < 0 || n >= area.map.size())
      return invalid;
    return area.map[n];
  }

  auto value(rect_area const area, vec2z const position,
             int8_t const invalid) noexcept -> int8_t {
    auto const n = index_of(area, position);
    if (n < 0)
      return invalid;
    return area.map[n];
  }

  auto neighbors4(vec2z const position, sl::index const n,
                  fn_available const available, intval const scale,
                  rect_area const area) noexcept -> link {

    if (!contains(area, position))
      return {};

    if (!available(value(area, position)))
      return {};

    auto const check = [&](sl::index x, sl::index y) -> link {
      auto const n = index_of(area, { x, y });

      if (n < 0 || !available(value(area, n)))
        return { .node = link::skip };

      return link { .node = n, .distance = scale };
    };

    switch (n) {
      case 0: return check(position.x(), position.y() - 1);
      case 1: return check(position.x(), position.y() + 1);
      case 2: return check(position.x() - 1, position.y());
      case 3: return check(position.x() + 1, position.y());
    }

    return {};
  }

  auto neighbors8(vec2z const position, sl::index const n,
                  fn_available const available,
                  intval const       scale_ortho,
                  intval const       scale_diagonal,
                  rect_area const    area) noexcept -> link {

    if (!contains(area, position))
      return {};

    if (!available(value(area, position)))
      return {};

    auto const check = [&](sl::index x, sl::index y,
                           intval distance) -> link {
      auto const n = index_of(area, { x, y });

      if (n < 0 || !available(value(area, n)))
        return { .node = link::skip };

      return link { .node = n, .distance = distance };
    };

    switch (n) {
      case 0:
        return check(position.x(), position.y() - 1, scale_ortho);
      case 1:
        return check(position.x(), position.y() + 1, scale_ortho);
      case 2:
        return check(position.x() - 1, position.y(), scale_ortho);
      case 3:
        return check(position.x() + 1, position.y(), scale_ortho);
    }

    switch (n) {
      case 4:
        return check(position.x() - 1, position.y() - 1,
                     scale_diagonal);
      case 5:
        return check(position.x() + 1, position.y() - 1,
                     scale_diagonal);
      case 6:
        return check(position.x() - 1, position.y() + 1,
                     scale_diagonal);
      case 7:
        return check(position.x() + 1, position.y() + 1,
                     scale_diagonal);
    }

    return {};
  }

  auto manhattan(vec2z const a, vec2z const b,
                 intval const scale) noexcept -> intval {
    auto const dx = a.x() < b.x() ? sub(b.x(), a.x())
                                  : sub(a.x(), b.x());
    auto const dy = a.y() < b.y() ? sub(b.y(), a.y())
                                  : sub(a.y(), b.y());

    return mul(add(dx, dy), scale);
  }

  auto diagonal(vec2z const a, vec2z const b,
                intval const scale) noexcept -> intval {
    auto const dx = a.x() < b.x() ? sub(b.x(), a.x())
                                  : sub(a.x(), b.x());
    auto const dy = a.y() < b.y() ? sub(b.y(), a.y())
                                  : sub(a.y(), b.y());

    if (dx < dy)
      return add(mul(sqrt2(scale), dx), mul(sub(dy, dx), scale));

    return add(mul(sqrt2(scale), dy), mul(sub(dx, dy), scale));
  }

  auto euclidean(vec2z const a, vec2z const b,
                 intval const scale) noexcept -> intval {
    auto const dx = a.x() < b.x() ? sub(b.x(), a.x())
                                  : sub(a.x(), b.x());
    auto const dy = a.y() < b.y() ? sub(b.y(), a.y())
                                  : sub(a.y(), b.y());

    return eval::sqrt(mul(add(sqr(dx), sqr(dy)), scale), scale);
  }

  auto path_exists(vec2z const source, vec2z const destination,
                   fn_available const available,
                   rect_area const    area) noexcept -> bool {

    if (area.stride <= 0 || area.width <= 0)
      return false;

    if (!contains(area, source))
      return false;

    if (!contains(area, destination))
      return false;

    if (trace_line(source - area.origin, destination - area.origin,
                   [&](vec2z const p) {
                     return contains(area, p) &&
                            available(value(area, p));
                   })) {
      return true;
    }

    auto const sight = [](sl::index const, sl::index const) {
      return false;
    };

    auto const heuristic = [&](sl::index const a,
                               sl::index const b) -> intval {
      return manhattan(point_of(area.stride, a),
                       point_of(area.stride, b), 1);
    };

    auto const neighbors = [&](sl::index const p,
                               sl::index const n) -> link {
      return neighbors8(point_of(area, p), n, available, 1, 1, area);
    };

    auto state = astar::init<false, astar::_basic_node>(
        index_of(area, source), index_of(area, destination));

    for (;;) {
      auto const result = astar::loop(sight, neighbors, heuristic,
                                      state);

      if (result == astar::status::success)
        return true;

      if (result == astar::status::failed)
        break;
    }

    return false;
  }

  auto path_search_init(vec2z const source, vec2z const destination,
                        fn_available const available,
                        intval const       scale,
                        rect_area const    area) noexcept -> _state {

    if (area.stride <= 0 || area.width <= 0)
      return {};

    auto s = _state {};

    s.astar = astar::init<true, astar::_node_theta>(
        index_of(area, source), index_of(area, destination));

    s.origin = area.origin;
    s.stride = area.stride;

    s.heuristic = [stride = area.stride,
                   scale](sl::index const a,
                          sl::index const b) -> intval {
      return euclidean(point_of(stride, a), point_of(stride, b),
                       scale);
    };

    s.neighbors = [area, available, scale,
                   scale_diagonal = sqrt2(scale)](
                      sl::index const p, sl::index const n) -> link {
      return neighbors8(point_of(area, p), n, available, scale,
                        scale_diagonal, area);
    };

    s.sight = [available, area](sl::index const a,
                                sl::index const b) -> bool {
      auto const p0 = point_of(area, a);
      auto const p1 = point_of(area, b);

      return trace_line(p0, p1, [&](vec2z const p) {
        return available(value(area, p));
      });
    };

    return s;
  }

  auto path_search_loop(_state &state) noexcept -> astar::status {

    return astar::loop(state.sight, state.neighbors, state.heuristic,
                       state.astar);
  }

  auto path_search_finish(_state const &state) noexcept
      -> sl::vector<vec2z> {
    if (state.stride <= 0) {
      return {};
    }

    auto const v = astar::finish<astar::_node_theta>(
        state.astar.closed, state.astar.source, state.astar.nearest);

    auto path = sl::vector<vec2z> {};
    path.reserve(v.size());

    for (auto &p : v)
      path.emplace_back(state.origin + point_of(state.stride, p));

    return path;
  }

  void convolve(vec2z const size, span<int8_t> const dst,
                span<int8_t const> const src, vec2z const fp_size,
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
        if (src[j * size.x() + i] <= 0)
          continue;

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
            if (footprint[y * fp_size.x() + x] <= 0)
              continue;

            dst[(j0 + y) * size.x() + i0 + x] = 1;
          }
      }
  }

  auto nearest(vec2z const position, vec2z const size,
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

    if (map[y0 * size.x() + x0] <= 0)
      return { x0, y0 };

    auto x        = x0;
    auto y        = y0;
    auto distance = sl::index { -1 };

    auto do_point = [&](sl::index const i, sl::index const j) {
      if (!condition(map[j * size.x() + i]))
        return;

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

      if (0 < distance && distance <= sqr(radius))
        break;
    }

    return { x, y };
  }
}
