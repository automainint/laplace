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
#include "polygon.h"
#include "quad.h"
#include "ray.h"
#include "shape.h"

namespace laplace::engine::tridi {
  using std::abs, std::min, std::max;

  auto intersects(box const &a, box const &b) -> bool {
    return a.max.x() + epsilon > b.min.x() &&
           a.max.y() + epsilon > b.min.y() &&
           a.max.z() + epsilon > b.min.z() &&
           b.max.x() + epsilon > a.min.x() &&
           b.max.y() + epsilon > a.min.y() &&
           b.max.z() + epsilon > a.min.z();
  }

  auto intersects(box const &a, cylinder const &b) -> bool {
    auto contains_flat = [](intval x, intval y,
                            const cylinder &b) -> bool {
      if (auto dx = abs(b.base.x() - x); dx - epsilon <= b.radius)
        if (auto dy = abs(b.base.y() - y); dy - epsilon <= b.radius) {
          auto r = dx * dx + dy * dy;
          return r - epsilon <= b.radius * b.radius;
        }
      return false;
    };
    if (a.max.z() + epsilon <= b.base.z())
      return false;
    if (a.min.z() - epsilon >= b.base.z() + b.height)
      return false;
    if (contains_flat(a.min.x(), a.min.y(), b))
      return true;
    if (contains_flat(a.min.x(), a.max.y(), b))
      return true;
    if (contains_flat(a.max.x(), a.min.y(), b))
      return true;
    if (contains_flat(a.max.x(), a.max.y(), b))
      return true;
    return false;
  }

  auto intersects(box const &a, sphere const &b) -> bool {
    if (a.max.x() + epsilon <= b.center.x() - b.radius)
      return false;
    if (a.max.y() + epsilon <= b.center.y() - b.radius)
      return false;
    if (a.max.z() + epsilon <= b.center.z() - b.radius)
      return false;
    if (a.min.x() - epsilon >= b.center.x() + b.radius)
      return false;
    if (a.min.y() - epsilon >= b.center.y() + b.radius)
      return false;
    if (a.min.z() - epsilon >= b.center.z() + b.radius)
      return false;
    for (sl::index i = 0; i < 8; i++)
      if (b.contains(a.get_vertex(i)))
        return true;
    return false;
  }

  auto intersects(box const &a, triangle const &b) -> bool {
    auto bounds = b.get_bounds();
    if (!intersects(a, bounds))
      return false;

    auto v0_ori = b.orientation(a.get_vertex(0));
    if (abs(v0_ori) <= epsilon)
      return true;

    for (sl::index i = 1; i < 8; i++) {
      auto v_ori = b.orientation(a.get_vertex(i));
      if (abs(v_ori) <= epsilon)
        return true;
      if ((v0_ori > 0) != (v_ori > 0))
        return true;
    }

    return false;
  }

  auto intersects(box const &a, octree const &b) -> bool {
    if (!intersects(a, b.bounds))
      return false;
    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs)) {
        if (intersects(a, c))
          return true;
      }
    } else {
      for (auto const &tr : get<1>(b.childs)) {
        if (intersects(a, tr))
          return true;
      }
    }
    return false;
  }

  auto intersects(cylinder const &a, cylinder const &b) -> bool {
    if (a.base.z() - epsilon >= b.base.z() + b.height)
      return false;
    if (a.base.z() + a.height + epsilon <= b.base.z())
      return false;
    auto d = a.radius + b.radius;
    if (auto dx = abs(a.base.x() - b.base.x()); dx - epsilon <= d)
      if (auto dy = abs(a.base.y() - b.base.y()); dy - epsilon <= d) {
        auto r = dx * dx + dy * dy;
        return r - epsilon <= d;
      }
    return false;
  }

  auto intersects(cylinder const &a, sphere const &b) -> bool {
    if (a.base.z() - epsilon >= b.center.z() + b.radius)
      return false;
    if (a.base.z() + a.height + epsilon <= b.center.z() - b.radius)
      return false;

    auto dx = intval {};
    auto dy = intval {};
    auto d  = a.radius + b.radius;

    if (dx = abs(a.base.x() - b.center.x()); dx - epsilon >= d)
      return false;
    if (dy = abs(a.base.y() - b.center.y()); dy - epsilon >= d)
      return false;
    auto r = dx * dx + dy * dy;
    if (r - epsilon >= d)
      return false;

    auto l = eval::sqrt(r, 1) - math::round<intval>(a.radius);
    auto h = a.radius * a.radius - l * l;
    if (a.base.z() - epsilon >= b.center.z() + h)
      return false;
    if (a.base.z() + a.height + epsilon <= b.center.z() - h)
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

    if (a.base.z() + epsilon >= b.vertices[0].z() &&
        a.base.z() + epsilon >= b.vertices[1].z() &&
        a.base.z() + epsilon >= b.vertices[2].z())
      return false;

    if (a.base.z() + a.height - epsilon <= b.vertices[0].z() &&
        a.base.z() + a.height - epsilon <= b.vertices[1].z() &&
        a.base.z() + a.height - epsilon <= b.vertices[2].z())
      return false;

    auto stem = ray { a.base, { 0, 0, 1 } };
    auto d    = a.radius * a.radius;

    if (stem.square_distance(
            ray { b.vertices[0], b.vertices[1] - b.vertices[0] }) -
            epsilon <=
        d)
      return true;
    if (stem.square_distance(
            ray { b.vertices[1], b.vertices[2] - b.vertices[1] }) -
            epsilon <=
        d)
      return true;
    if (stem.square_distance(
            ray { b.vertices[2], b.vertices[0] - b.vertices[2] }) -
            epsilon <=
        d)
      return true;

    return intersection(stem, b.get_plane()) + epsilon >= 0;
  }

  auto intersects(cylinder const &a, octree const &b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs)) {
        if (intersects(a, c)) {
          return true;
        }
      }
    } else {
      for (auto const &tr : get<1>(b.childs)) {
        if (intersects(a, tr)) {
          return true;
        }
      }
    }
    return false;
  }

  auto intersects(sphere const &a, sphere const &b) -> bool {
    auto d = math::square_length(a.center - b.center);
    return d - epsilon <= a.radius + b.radius;
  }

  auto intersects(sphere const &a, triangle const &b) -> bool {
    return a.contains(b.vertices[0]) || a.contains(b.vertices[1]) ||
           a.contains(b.vertices[2]) ||
           b.plane_abs_square_distance(a.center) - epsilon <=
               a.radius * a.radius;
  }

  auto intersects(sphere const &a, octree const &b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs))
        if (intersects(a, c))
          return true;

    } else {
      for (auto const &tr : get<1>(b.childs))
        if (intersects(a, tr))
          return true;
    }
    return false;
  }

  auto intersects(triangle const &a, triangle const &b) -> bool {
    auto ori0 = b.orientation(a.vertices[0]);
    auto ori1 = b.orientation(a.vertices[1]);
    auto ori2 = b.orientation(a.vertices[2]);

    if (abs(ori0) >= epsilon && abs(ori1) >= epsilon &&
        abs(ori2) >= epsilon && (ori0 < 0) == (ori1 < 0) &&
        (ori0 < 0) == (ori2 < 0))
      return false;

    return a.contains_flat(b.vertices[0]) ||
           a.contains_flat(b.vertices[1]) ||
           a.contains_flat(b.vertices[2]) ||
           b.contains_flat(a.vertices[0]) ||
           b.contains_flat(a.vertices[1]) ||
           b.contains_flat(a.vertices[2]);
  }

  auto intersects(triangle const &a, octree const &b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs))
        if (intersects(a, c))
          return true;

    } else {
      for (auto const &tr : get<1>(b.childs))
        if (intersects(a, tr))
          return true;
    }
    return false;
  }

  auto intersects(octree const &a, octree const &b) -> bool {
    if (!intersects(a.bounds, b.bounds))
      return false;
    if (a.childs.index() == 0) {
      auto const &childs = get<0>(a.childs);
      for (auto const &c : childs)
        if (intersects(c, b))
          return true;
    } else if (b.childs.index() == 0) {
      auto const &childs = get<0>(b.childs);
      for (auto const &c : childs)
        if (intersects(a, c))
          return true;
    } else {
      auto const &v0 = get<1>(a.childs);
      auto const &v1 = get<1>(b.childs);

      for (auto const &tr0 : v0)
        if (intersects(b.bounds, tr0))
          for (auto const &tr1 : v1)
            if (intersects(tr0, tr1))
              return true;
    }
    return false;
  }

  auto intersects(ray const &a, plane const &b) -> bool {
    const auto r     = b.base - a.base;
    const auto r_cos = math::dot(r, b.normal);
    const auto n     = r_cos < 0 ? b.normal : -b.normal;
    const auto v     = n * math::dot(n, a.direction);

    return v.x() * v.x() >= epsilon || v.y() * v.y() >= epsilon ||
           v.z() * v.z() >= epsilon;
  }

  auto intersects(ray const &a, triangle const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t + epsilon >= 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, quad const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t + epsilon >= 0 && b.contains_flat(a.point(t));
  }

  auto intersects(ray const &a, polygon const &b) -> bool {
    auto t = intersection(a, b.get_plane());
    return t + epsilon >= 0 && b.contains_flat(a.point(t));
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
    auto rr = cyl.radius * cyl.radius;

    if (ra.square_distance(ray { cyl.base, vec3i { 0, 0, 1 } }) +
            epsilon >=
        rr)
      return false;

    auto t0 = intersection(ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    if (t0 + epsilon >= 0 && cyl.contains_flat(ra.point(t0)))
      return true;

    auto t1 = intersection(
        ra, plane { vec3i { cyl.base.x(), cyl.base.y(),
                            cyl.base.z() + cyl.height },
                    vec3i { 0, 0, 1 } });

    return t1 + epsilon >= 0 && cyl.contains_flat(ra.point(t1));
  }

  auto intersects(ray const &ra, sphere const &sph) -> bool {
    return ra.square_distance(sph.center) - epsilon <=
           sph.radius * sph.radius;
  }

  auto intersects(ray const &a, octree const &b) -> bool {
    if (!intersects(a, b.bounds))
      return false;

    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs))
        if (intersects(a, c))
          return true;

    } else {
      for (auto const &tr : get<1>(b.childs))
        if (intersects(a, tr))
          return true;
    }

    return false;
  }

  auto intersection(ray const &a, plane const &b) -> intval {
    const auto r     = b.base - a.base;
    const auto r_cos = math::dot(r, b.normal);
    const auto n     = r_cos < 0 ? b.normal : -b.normal;
    const auto k     = length(n * math::dot(n, a.direction));

    if (k != 0) {
      const auto h = length(n * r_cos);
      const auto d = length(a.direction);

      return eval::div(h * d, k, 1);
    }

    return -safe_limit;
  }

  auto intersection(ray const &a, triangle const &b) -> intval {
    auto t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      t = -safe_limit;

    return t;
  }

  auto intersection(ray const &a, quad const &b) -> intval {
    auto t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      t = -safe_limit;

    return t;
  }

  auto intersection(ray const &a, polygon const &b) -> intval {
    auto t = intersection(a, b.get_plane());

    if (t > -infinity && !b.contains_flat(a.point(t)))
      t = -safe_limit;

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

      return eval::div(-c, b, 1);
    }

    /*  Quadratic solution.
     */

    auto d = b * b - 4 * a * c;

    if (d < 0) {
      return -safe_limit;
    }

    auto delta = eval::sqrt(d, 1);

    return eval::div(b <= 0 && -b >= delta ? -b - delta : -b + delta,
                     2 * a, 1);
  }

  auto intersection(ray const &ra, cylinder const &cyl) -> intval {
    auto rr = cyl.radius * cyl.radius;

    if (ra.square_distance(ray { cyl.base, vec3i { 0, 0, 1 } }) +
            epsilon >=
        rr) {
      return -safe_limit;
    }

    auto const t0 = intersection(
        ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    auto const t1 = intersection(
        ra, plane { vec3i { cyl.base.x(), cyl.base.y(),
                            cyl.base.z() + cyl.height },
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

    auto const dx = x0 - cx;
    auto const dy = y0 - cy;

    /*  Intersection condition:
     *
     *      (vx t + dx)^2 + (vy t + dy)^2 - r^2 = 0
     *
     *      (vx^2 + vy^2) t^2 + 2 (vx dx + vy dy) t + (dx^2 +
     * dy^2 - r^2) = 0
     */

    auto const t = quadratic_solution(vx * vx + vy * vy,
                                      2 * (vx * dx + vy * dy),
                                      dx * dx + dy * dy - rr);

    if (t > -safe_limit) {
      auto const z = ra.base.z() + ra.direction.z() * t;

      if (z >= cyl.base.z() - epsilon ||
          z <= cyl.base.z() + cyl.height + epsilon)
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
    auto const rr = sph.radius * sph.radius;

    if (ra.square_distance(sph.center) + epsilon >= rr)
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

    auto const dx = x0 - cx;
    auto const dy = y0 - cy;
    auto const dz = z0 - cz;

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

    return quadratic_solution(vx * vx + vy * vy + vz * vz,
                              2 * (vx * dx + vy * dy + vz * vz),
                              dx * dx + dy * dy + dz * dz - rr);
  }

  auto intersection(ray const &a, octree const &b) -> intval {
    if (!intersects(a, b.bounds))
      return -safe_limit;

    if (b.childs.index() == 0) {
      for (auto const &c : get<0>(b.childs)) {
        auto t = intersection(a, c);

        if (t + epsilon >= 0)
          return t;
      }
    } else {
      auto result = safe_limit;
      for (auto const &tr : get<1>(b.childs)) {
        auto t = intersection(a, tr);

        if (t + epsilon >= 0 && t < result)
          result = t;
      }
      if (result < infinity)
        return result;
    }

    return -safe_limit;
  }

  auto intersects(shape const &a, shape const &b) -> bool {
    if (!a.is_empty() && !b.is_empty()) {
      if (a.is_box()) {
        if (b.is_box())
          return intersects(a.get_box(), b.get_box());
        if (b.is_cylinder())
          return intersects(a.get_box(), b.get_cylinder());
        if (b.is_sphere())
          return intersects(a.get_box(), b.get_sphere());
        if (b.is_octree())
          return intersects(a.get_box(), b.get_octree());
      }

      if (a.is_cylinder()) {
        if (b.is_box())
          return intersects(b.get_box(), a.get_cylinder());
        if (b.is_cylinder())
          return intersects(a.get_cylinder(), b.get_cylinder());
        if (b.is_sphere())
          return intersects(a.get_cylinder(), b.get_sphere());
        if (b.is_octree())
          return intersects(a.get_cylinder(), b.get_octree());
      }

      if (a.is_sphere()) {
        if (b.is_box())
          return intersects(b.get_box(), a.get_sphere());
        if (b.is_cylinder())
          return intersects(b.get_cylinder(), a.get_sphere());
        if (b.is_sphere())
          return intersects(a.get_sphere(), b.get_sphere());
        if (b.is_octree())
          return intersects(a.get_sphere(), b.get_octree());
      }

      if (a.is_octree()) {
        if (b.is_box())
          return intersects(b.get_box(), a.get_octree());
        if (b.is_cylinder())
          return intersects(b.get_cylinder(), a.get_octree());
        if (b.is_sphere())
          return intersects(b.get_sphere(), a.get_octree());
        if (b.is_octree())
          return intersects(a.get_octree(), b.get_octree());
      }
    }

    return false;
  }

  auto intersects(ray const &a, shape const &b) -> bool {
    return b.is_box()        ? intersects(a, b.get_box())
           : b.is_cylinder() ? intersects(a, b.get_cylinder())
           : b.is_sphere()
               ? intersects(a, b.get_sphere())
               : b.is_octree() && intersects(a, b.get_octree());
  }

  auto intersection(ray const &a, shape const &b) -> int64_t {
    return b.is_box()        ? intersection(a, b.get_box())
           : b.is_cylinder() ? intersection(a, b.get_cylinder())
           : b.is_sphere()   ? intersection(a, b.get_sphere())
           : b.is_octree()   ? intersection(a, b.get_octree())
                             : -safe_limit;
  }
}
