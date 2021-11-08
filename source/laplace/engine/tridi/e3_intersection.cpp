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

  auto intersects(box const &a, box const &b) -> bool {
    return a.max.x() >= b.min.x() && a.max.y() >= b.min.y() &&
           a.max.z() >= b.min.z() && b.max.x() >= a.min.x() &&
           b.max.y() >= a.min.y() && b.max.z() >= a.min.z();
  }

  auto intersects(box const &a, cylinder const &b) -> bool {
    auto contains_flat = [&](intval x, intval y) -> bool {
      if (auto dx = abs(sub(b.base.x(), x)); dx <= b.radius)
        if (auto dy = abs(sub(b.base.y(), y)); dy <= b.radius)
          return add(sqr(dx), sqr(dy)) <= sqr(b.radius);
      return false;
    };
    if (a.max.z() < b.base.z())
      return false;
    if (a.min.z() > add(b.base.z(), b.height))
      return false;
    if (contains_flat(a.min.x(), a.min.y()))
      return true;
    if (contains_flat(a.min.x(), a.max.y()))
      return true;
    if (contains_flat(a.max.x(), a.min.y()))
      return true;
    if (contains_flat(a.max.x(), a.max.y()))
      return true;
    if (b.base.x() >= a.min.x() && b.base.x() <= a.max.x()) {
      if (auto const y = add(b.base.y(), b.radius);
          y >= a.min.y() && y <= a.max.y())
        return true;
      if (auto const y = sub(b.base.y(), b.radius);
          y >= a.min.y() && y <= a.max.y())
        return true;
    }
    if (b.base.y() >= a.min.y() && b.base.y() <= a.max.y()) {
      if (auto const x = add(b.base.x(), b.radius);
          x >= a.min.x() && x <= a.max.x())
        return true;
      if (auto const x = sub(b.base.x(), b.radius);
          x >= a.min.x() && x <= a.max.x())
        return true;
    }
    return false;
  }

  auto intersects(box const &a, sphere const &b) -> bool {
    if (a.max.x() < sub(b.center.x(), b.radius))
      return false;
    if (a.max.y() < sub(b.center.y(), b.radius))
      return false;
    if (a.max.z() < sub(b.center.z(), b.radius))
      return false;
    if (a.min.x() > add(b.center.x(), b.radius))
      return false;
    if (a.min.y() > add(b.center.y(), b.radius))
      return false;
    if (a.min.z() > add(b.center.z(), b.radius))
      return false;
    for (sl::index i = 0; i < 8; i++)
      if (b.contains(a.get_vertex(i)))
        return true;
    return false;
  }

  auto intersects(box const &a, triangle const &b) -> bool {
    auto const bounds = b.get_bounds();
    if (!intersects(a, bounds))
      return false;

    auto const v0_ori = b.orientation(a.get_vertex(0));
    if (v0_ori == 0)
      return true;

    for (sl::index i = 1; i < 8; i++) {
      auto const v_ori = b.orientation(a.get_vertex(i));
      if (v_ori == 0)
        return true;
      if ((v0_ori > 0) != (v_ori > 0))
        return true;
    }

    return false;
  }

  auto intersects(cylinder const &a, cylinder const &b) -> bool {
    if (a.base.z() > add(b.base.z(), b.height))
      return false;
    if (add(a.base.z(), a.height) < b.base.z())
      return false;
    if (auto const dx = abs(sub(a.base.x(), b.base.x()));
        dx <= a.radius)
      if (auto const dy = abs(sub(a.base.y(), b.base.y()));
          dy <= a.radius)
        return add(sqr(dx), sqr(dy)) <= sqr(a.radius);
    return false;
  }

  auto intersects(cylinder const &a, sphere const &b) -> bool {
    if (a.base.z() > add(b.center.z(), b.radius))
      return false;
    if (add(a.base.z(), a.height) < sub(b.center.z(), b.radius))
      return false;

    auto const d = add(a.radius, b.radius);

    auto const dx = abs(sub(a.base.x(), b.center.x()));
    if (dx > d)
      return false;

    auto const dy = abs(sub(a.base.y(), b.center.y()));
    if (dy > d)
      return false;

    auto const r = add(sqr(dx), sqr(dy));
    if (r > d)
      return false;

    auto const l = sub(sqrt(r, 1), a.radius);
    auto const h = sub(sqr(a.radius), sqr(l));
    if (a.base.z() > add(b.center.z(), h))
      return false;
    if (add(a.base.z(), a.height) < sub(b.center.z(), h))
      return false;

    return true;
  }

  auto intersects(cylinder const &a, triangle const &b) -> bool {
    if (a.contains(b.vertices[0]))
      return true;
    if (a.contains(b.vertices[1]))
      return true;
    if (a.contains(b.vertices[2]))
      return true;

    if (a.base.z() > b.vertices[0].z() &&
        a.base.z() > b.vertices[1].z() &&
        a.base.z() > b.vertices[2].z())
      return false;

    if (add(a.base.z(), a.height) < b.vertices[0].z() &&
        add(a.base.z(), a.height) < b.vertices[1].z() &&
        add(a.base.z(), a.height) < b.vertices[2].z())
      return false;

    auto stem = ray { a.base, { 0, 0, 1 } };
    auto d    = sqr(a.radius);

    if (stem.square_distance(ray {
            b.vertices[0], sub(b.vertices[1], b.vertices[0]) }) <= d)
      return true;
    if (stem.square_distance(ray {
            b.vertices[1], sub(b.vertices[2], b.vertices[1]) }) <= d)
      return true;
    if (stem.square_distance(ray {
            b.vertices[2], sub(b.vertices[0], b.vertices[2]) }) <= d)
      return true;

    return intersection(stem, b.get_plane()) > 0;
  }

  auto intersects(sphere const &a, sphere const &b) -> bool {
    auto d = square_length(sub(a.center, b.center));
    return d <= add(a.radius, b.radius);
  }

  auto intersects(sphere const &a, triangle const &b) -> bool {
    return a.contains(b.vertices[0]) || a.contains(b.vertices[1]) ||
           a.contains(b.vertices[2]) ||
           b.plane_abs_square_distance(a.center) <= sqr(a.radius);
  }

  auto intersects(triangle const &a, triangle const &b) -> bool {
    auto const ori0 = b.orientation(a.vertices[0]);
    auto const ori1 = b.orientation(a.vertices[1]);
    auto const ori2 = b.orientation(a.vertices[2]);

    if (ori0 != 0 && ori1 != 0 && ori2 != 0 &&
        (ori0 < 0) == (ori1 < 0) && (ori0 < 0) == (ori2 < 0))
      return false;

    return a.contains_flat(b.vertices[0]) ||
           a.contains_flat(b.vertices[1]) ||
           a.contains_flat(b.vertices[2]) ||
           b.contains_flat(a.vertices[0]) ||
           b.contains_flat(a.vertices[1]) ||
           b.contains_flat(a.vertices[2]);
  }

  auto intersects(ray const &a, plane const &b) -> bool {
    auto const r     = sub(b.base, a.base);
    auto const r_cos = dot(r, b.normal);
    auto const n     = r_cos < 0 ? b.normal : -b.normal;
    auto const v     = mul(n, dot(n, a.direction));

    return v.x() != 0 || v.y() != 0 || v.z() != 0;
  }

  auto intersects(ray const &a, triangle const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t > 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, quad const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t > 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, polygon const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t > 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, box const &b) -> bool {
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

  auto intersects(ray const &ra, cylinder const &cyl) -> bool {
    auto const rr = sqr(cyl.radius);

    if (ra.square_distance(ray { cyl.base, vec3i { 0, 0, 1 } }) > rr)
      return false;

    auto const t0 = intersection(
        ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    if (t0 > 0 && cyl.contains_flat(ra.point(t0)))
      return true;

    auto const t1 = intersection(
        ra, plane { vec3i { cyl.base.x(), cyl.base.y(),
                            add(cyl.base.z(), cyl.height) },
                    vec3i { 0, 0, 1 } });

    return t1 > 0 && cyl.contains_flat(ra.point(t1));
  }

  auto intersects(ray const &ra, sphere const &sph) -> bool {
    return ra.square_distance(sph.center) <= sqr(sph.radius);
  }

  auto intersection(ray const &a, plane const &b) -> intval {
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

  auto intersection(ray const &a, triangle const &b) -> intval {
    auto const t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      return -safe_limit;

    return t;
  }

  auto intersection(ray const &a, quad const &b) -> intval {
    auto const t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      return -safe_limit;

    return t;
  }

  auto intersection(ray const &a, polygon const &b) -> intval {
    auto const t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      return -safe_limit;

    return t;
  }

  auto intersection(ray const &a, box const &b) -> intval {
    if (b.contains(a.base)) {
      auto const t0 = max(intersection(a, b.get_plane(0)),
                          intersection(a, b.get_plane(1)));
      auto const t1 = max(intersection(a, b.get_plane(2)),
                          intersection(a, b.get_plane(3)));
      auto const t2 = max(intersection(a, b.get_plane(4)),
                          intersection(a, b.get_plane(5)));

      return min(t0, min(t1, t2));
    }

    auto const t0 = min(intersection(a, b.get_plane(0)),
                        intersection(a, b.get_plane(1)));
    auto const t1 = min(intersection(a, b.get_plane(2)),
                        intersection(a, b.get_plane(3)));
    auto const t2 = min(intersection(a, b.get_plane(4)),
                        intersection(a, b.get_plane(5)));

    auto const t = max(t0, max(t1, t2));
    auto const p = a.point(t);

    return b.contains(p) ? t : -safe_limit;
  }

  /*  Minimal nonnegative equation root, or nearest to
   *  zero root, if there is no nonnegative roots.
   *
   *  The equation:
   *      a x^2 + b x + c = 0
   */
  static auto quadratic_solution(intval a, intval b, intval c)
      -> intval {
    if (a == 0) {
      /*  Linear solution.
       */

      if (b == 0) {
        /*  No intersection.
         */

        return -safe_limit;
      }

      return div(-c, b, 1);
    }

    /*  Quadratic solution.
     */

    auto const d = sub(mul(b, b), mul(4, mul(a, c)));

    if (d < 0)
      return -safe_limit;

    auto const delta = sqrt(d, 1);

    return div(b <= 0 && -b >= delta ? sub(-b, delta)
                                     : add(-b, delta),
               add(a, 2));
  }

  auto intersection(ray const &ra, cylinder const &cyl) -> intval {
    auto rr = sqr(cyl.radius);

    if (ra.square_distance(ray { cyl.base, vec3i { 0, 0, 1 } }) > rr)
      return -safe_limit;

    auto const t0 = intersection(
        ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    auto const t1 = intersection(
        ra, plane { vec3i { cyl.base.x(), cyl.base.y(),
                            add(cyl.base.z(), cyl.height) },
                    vec3i { 0, 0, 1 } });

    auto const p0 = ra.point(t0);
    auto const p1 = ra.point(t1);

    auto const contains_p0 = cyl.contains(p0);
    auto const contains_p1 = cyl.contains(p1);

    if (t0 >= 0 && contains_p0)
      return t0;
    if (t1 >= 0 && contains_p1)
      return t1;

    auto const cx = cyl.base.x();
    auto const cy = cyl.base.y();

    auto const x0 = ra.base.x();
    auto const y0 = ra.base.y();

    auto const vx = ra.direction.x();
    auto const vy = ra.direction.y();

    /*  Circle in 2D:
     *
     *      (x - cx)^2 + (y - cy)^2 - r^2 = 0
     *
     *  Ray in 2D:
     *
     *      x = x0 + vx t
     *      y = y0 + vy t
     *
     *  Intersection condition:
     *
     *      (vx t + x0 - cx)^2 + (vy t + y0 - cy)^2 - r^2 = 0
     */

    auto const dx = sub(x0, cx);
    auto const dy = sub(y0, cy);

    /*  Intersection condition:
     *
     *      (vx t + dx)^2 + (vy t + dy)^2 - r^2 = 0
     *
     *      (vx^2 + vy^2) t^2 + 2 (vx dx + vy dy) t + (dx^2 +
     *       dy^2 - r^2) = 0
     */

    auto const t = quadratic_solution(
        add(sqr(vx), sqr(vy)), mul(2, add(mul(vx, dx), mul(vy, dy))),
        sub(add(sqr(dx), sqr(dy)), rr));

    if (t > -safe_limit) {
      auto const z = add(ra.base.z(), mul(ra.direction.z(), t));

      if (z >= cyl.base.z() || z <= add(cyl.base.z(), cyl.height))
        return t;

      if (t0 > t1) {
        if (contains_p0)
          return t0;
        if (contains_p1)
          return t1;
      } else {
        if (contains_p1)
          return t1;
        if (contains_p0)
          return t0;
      }
    }

    return -safe_limit;
  }

  auto intersection(ray const &ra, sphere const &sph) -> intval {
    auto const rr = sqr(sph.radius);

    if (ra.square_distance(sph.center) > rr)
      return -safe_limit;

    auto const cx = sph.center.x();
    auto const cy = sph.center.y();
    auto const cz = sph.center.z();

    auto const x0 = ra.base.x();
    auto const y0 = ra.base.y();
    auto const z0 = ra.base.z();

    auto const vx = ra.direction.x();
    auto const vy = ra.direction.y();
    auto const vz = ra.direction.z();

    /*  Sphere:
     *
     *      (x - cx)^2 + (y - cy)^2 + (z - cz)^2 - r^2 = 0
     *
     *  Ray:
     *
     *      x = x0 + vx t
     *      y = y0 + vy t
     *      z = z0 + vz t
     *
     *  Intersection condition:
     *
     *      (vx t + x0 - cx)^2 +
     *      (vy t + y0 - cy)^2 +
     *      (vz t + z0 - cz)^2 - r^2 = 0
     */

    auto const dx = sub(x0, cx);
    auto const dy = sub(y0, cy);
    auto const dz = sub(z0, cz);

    /*  Intersection condition:
     *
     *    (vx t + dx)^2 +
     *    (vy t + dy)^2 +
     *    (vz t + dz)^2 -
     *    r^2 = 0
     *
     *    (vx^2 + vy^2 + vz^2) t^2 +
     *    2 (vx dx + vy dy + vz dz) t +
     *    (dx^2 + dy^2 + dz^2 - r^2) = 0
     */

    return quadratic_solution(
        add(sqr(vx), add(sqr(vy), sqr(vz))),
        mul(2, add(mul(vx, dx), add(mul(vy, dy), mul(vz, vz)))),
        sub(add(sqr(dx), add(sqr(dy), sqr(dz))), rr));
  }
}
