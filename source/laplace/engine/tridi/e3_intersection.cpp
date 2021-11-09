/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "intersection.h"

#include "../eval/integral.h"
#include "box.h"
#include "cylinder.h"
#include "polygon.h"
#include "quad.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"
#include "vector.h"

namespace laplace::engine::tridi {
  using std::abs, std::min, std::max, eval::sqr, eval::sqrt,
      eval::add, eval::sub, eval::mul, eval::div;

  auto intersects(box const &a, box const &b) noexcept -> bool {
    return a.max.x() >= b.min.x() && a.max.y() >= b.min.y() &&
           a.max.z() >= b.min.z() && b.max.x() >= a.min.x() &&
           b.max.y() >= a.min.y() && b.max.z() >= a.min.z();
  }

  auto intersects(box const &bo, cylinder const &cyl) noexcept
      -> bool {
    auto contains = [&](intval x, intval y) -> bool {
      if (auto dx = abs(sub(cyl.base.x(), x)); dx <= cyl.radius)
        if (auto dy = abs(sub(cyl.base.y(), y)); dy <= cyl.radius)
          return add(sqr(dx), sqr(dy)) <= sqr(cyl.radius);
      return false;
    };
    if (bo.max.z() < cyl.get_floor_z() ||
        bo.min.z() > cyl.get_roof_z())
      return false;
    if (contains(bo.min.x(), bo.min.y()) ||
        contains(bo.min.x(), bo.max.y()) ||
        contains(bo.max.x(), bo.min.y()) ||
        contains(bo.max.x(), bo.max.y()))
      return true;
    if (cyl.base.x() >= bo.min.x() && cyl.base.x() <= bo.max.x()) {
      if (auto const y = add(cyl.base.y(), cyl.radius);
          y >= bo.min.y() && y <= bo.max.y())
        return true;
      if (auto const y = sub(cyl.base.y(), cyl.radius);
          y >= bo.min.y() && y <= bo.max.y())
        return true;
    }
    if (cyl.base.y() >= bo.min.y() && cyl.base.y() <= bo.max.y()) {
      if (auto const x = add(cyl.base.x(), cyl.radius);
          x >= bo.min.x() && x <= bo.max.x())
        return true;
      if (auto const x = sub(cyl.base.x(), cyl.radius);
          x >= bo.min.x() && x <= bo.max.x())
        return true;
    }
    return false;
  }

  auto intersects(box const &bo, sphere const &sph) noexcept -> bool {
    if (bo.max.x() < sub(sph.center.x(), sph.radius))
      return false;
    if (bo.max.y() < sub(sph.center.y(), sph.radius))
      return false;
    if (bo.max.z() < sub(sph.center.z(), sph.radius))
      return false;
    if (bo.min.x() > add(sph.center.x(), sph.radius))
      return false;
    if (bo.min.y() > add(sph.center.y(), sph.radius))
      return false;
    if (bo.min.z() > add(sph.center.z(), sph.radius))
      return false;
    auto const fx = (bo.min.x() <= sph.center.x() &&
                     bo.max.x() >= sph.center.x())
                        ? 1
                        : 0;
    auto const fy = (bo.min.y() <= sph.center.y() &&
                     bo.max.y() >= sph.center.y())
                        ? 1
                        : 0;
    auto const fz = (bo.min.z() <= sph.center.z() &&
                     bo.max.z() >= sph.center.z())
                        ? 1
                        : 0;
    if (fx + fy + fz >= 2)
      return true;
    for (sl::index i = 0; i < 8; i++)
      if (sph.contains(bo.get_vertex(i)))
        return true;
    return false;
  }

  auto intersects(box const &bo, triangle const &tri) noexcept
      -> bool {
    if (!intersects(bo, tri.get_bounds()))
      return false;

    return bo.contains(tri.vertices[0]) ||
           bo.contains(tri.vertices[1]) ||
           bo.contains(tri.vertices[2]) ||
           intersects(tri, bo.get_edge(0)) ||
           intersects(tri, bo.get_edge(1)) ||
           intersects(tri, bo.get_edge(2)) ||
           intersects(tri, bo.get_edge(3)) ||
           intersects(tri, bo.get_edge(4)) ||
           intersects(tri, bo.get_edge(5)) ||
           intersects(tri, bo.get_edge(6)) ||
           intersects(tri, bo.get_edge(7)) ||
           intersects(tri, bo.get_edge(8)) ||
           intersects(tri, bo.get_edge(9)) ||
           intersects(tri, bo.get_edge(10)) ||
           intersects(tri, bo.get_edge(11));
  }

  auto intersects(cylinder const &a, cylinder const &b) noexcept
      -> bool {
    if (a.get_floor_z() > b.get_roof_z() ||
        a.get_roof_z() < b.get_floor_z())
      return false;
    if (auto const dx = abs(sub(a.base.x(), b.base.x()));
        dx <= a.radius)
      if (auto const dy = abs(sub(a.base.y(), b.base.y()));
          dy <= a.radius)
        return add(sqr(dx), sqr(dy)) <= sqr(a.radius);
    return false;
  }

  auto intersects(cylinder const &cyl, sphere const &sph) noexcept
      -> bool {
    if (cyl.get_floor_z() > add(sph.center.z(), sph.radius) ||
        cyl.get_roof_z() < sub(sph.center.z(), sph.radius))
      return false;

    auto const r_max = add(cyl.radius, sph.radius);

    auto const dx = abs(sub(cyl.base.x(), sph.center.x()));
    if (dx > r_max)
      return false;

    auto const dy = abs(sub(cyl.base.y(), sph.center.y()));
    if (dy > r_max)
      return false;

    auto const r2 = add(sqr(dx), sqr(dy));
    if (r2 > sqr(r_max))
      return false;
    if (r2 <= sqr(cyl.radius))
      return true;

    auto const r      = sqrt(r2);
    auto const l2     = sub(add(sqr(cyl.radius), r2),
                            mul(mul(cyl.radius, r), 2));
    auto const h2     = sub(sqr(sph.radius), l2);
    auto const h_min0 = sub(sph.center.z(), cyl.get_roof_z());
    auto const h_min1 = sub(cyl.get_floor_z(), sph.center.z());

    if (h_min0 >= 0 && h2 < sqr(h_min0))
      return false;
    if (h_min1 >= 0 && h2 < sqr(h_min1))
      return false;

    return true;
  }

  auto intersects(sphere const &a, sphere const &b) noexcept -> bool {
    auto d = square_length(sub(a.center, b.center));
    return d <= add(a.radius, b.radius);
  }

  auto intersects(triangle const &tri, segment const &seg) noexcept
      -> bool {
    return intersects(seg.get_ray(0), tri) &&
           intersects(seg.get_ray(1), tri);
  }

  auto intersects(ray const &a, triangle const &b) noexcept -> bool {
    auto t = intersection(a, b.get_plane());
    return t > 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, box const &b) noexcept -> bool {
    if (b.contains(a.base))
      return true;

    auto const t0 = min(intersection(a, b.get_plane(0)),
                        intersection(a, b.get_plane(1)));
    auto const t1 = min(intersection(a, b.get_plane(2)),
                        intersection(a, b.get_plane(3)));
    auto const t2 = min(intersection(a, b.get_plane(4)),
                        intersection(a, b.get_plane(5)));

    auto const t = max(t0, max(t1, t2));
    auto const p = a.point(t);

    return b.contains(p);
  }

  auto intersection(ray const &a, plane const &b) noexcept -> intval {
    auto const r     = sub(b.base, a.base);
    auto const r_cos = dot(r, b.normal);
    auto const n     = r_cos < 0 ? b.normal : -b.normal;
    auto const k     = length(mul(n, dot(n, a.direction)));

    if (k != 0) {
      auto const h = length(mul(n, r_cos));
      auto const d = length(a.direction);

      return div(mul(h, d), k);
    }

    return -safe_limit;
  }
}
