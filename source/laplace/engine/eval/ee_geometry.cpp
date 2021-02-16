/*  laplace/engine/eval/ee_geometry.cpp
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

#include "geometry.h"
#include <algorithm>
#include <cassert>

namespace laplace::engine::eval {
  using std::min, std::max;

  void append(ref_box a, cref_vec3i point) {
    if (a.min.x > a.max.x || a.min.y > a.max.y ||
        a.min.z > a.max.z) {
      a.min = point;
      a.max = point;
    } else {
      if (a.min.x > point.x)
        a.min.x = point.x;
      if (a.min.y > point.y)
        a.min.y = point.y;
      if (a.min.z > point.z)
        a.min.z = point.z;
      if (a.max.x < point.x)
        a.max.x = point.x;
      if (a.max.y < point.y)
        a.max.y = point.y;
      if (a.max.z < point.z)
        a.max.z = point.z;
    }
  }

  void append(ref_box a, cref_triangle b) {
    append(a, b[0]);
    append(a, b[1]);
    append(a, b[2]);
  }

  void append(ref_box a, cref_quad b) {
    append(a, b[0]);
    append(a, b[1]);
    append(a, b[2]);
    append(a, b[3]);
  }

  void append(ref_box a, cref_polygon b) {
    for (auto &p : b) { append(a, p); }
  }

  auto is_empty(cref_box a) -> bool {
    return a.max.x < a.min.x || a.max.y < a.min.y ||
           a.max.z < a.min.z;
  }

  auto is_empty(cref_cylinder a) -> bool {
    return a.radius < 0 || a.height < 0;
  }

  auto is_empty(cref_sphere a) -> bool {
    return a.radius < 0;
  }

  auto is_empty(cref_octree a) -> bool {
    if (!is_empty(a.bounds) && a.childs.index() == 0) {
      for (size_t i = 0; i < get<0>(a.childs).size(); i++) {
        if (!is_empty(get<0>(a.childs)[i]))
          return false;
      }
    }

    return true;
  }

  auto vertex_of(cref_box a, size_t index) -> vec3i {
    if (index == 0)
      return a.min;
    if (index == 1)
      return vec3i(a.min.x, a.min.y, a.max.z);
    if (index == 2)
      return vec3i(a.min.x, a.max.y, a.min.z);
    if (index == 3)
      return vec3i(a.min.x, a.max.y, a.max.z);
    if (index == 4)
      return vec3i(a.max.x, a.min.y, a.min.z);
    if (index == 5)
      return vec3i(a.max.x, a.min.y, a.max.z);
    if (index == 6)
      return vec3i(a.max.x, a.max.y, a.min.z);
    return a.max;
  }

  auto center_of(cref_box a) -> vec3i {
    return (a.min + a.max) / 2ll;
  }

  auto center_of(cref_triangle a) -> vec3i {
    return vec3i { flat_center_of(a, 0), flat_center_of(a, 1),
                   flat_center_of(a, 2) };
  }

  auto point_of(cref_ray a, vecval t) -> vec3i {
    return a.base + (a.direction * t) / math::length(a.direction);
  }

  auto plane_of(cref_triangle a) -> plane {
    return plane { a[0], raw_normal(a) };
  }

  auto plane_of(cref_quad a) -> plane {
    return plane { a[0], raw_normal(a[0], a[1], a[2]) };
  }

  auto plane_of(cref_polygon a) -> plane {
    if (a.size() < 3)
      return plane { a[0], vec3i(0, 0, 0) };
    return plane { a[0], raw_normal(a[0], a[1], a[2]) };
  }

  auto plane_of(cref_box a, size_t index) -> plane {
    if (index == 0)
      return plane { a.min, vec3i(0, 0, -1) };
    if (index == 1)
      return plane { vec3i(a.min.x, a.min.y, a.max.z),
                     vec3i(0, 0, 1) };
    if (index == 2)
      return plane { vec3i(a.min.x, a.min.y, a.max.z),
                     vec3i(0, -1, 0) };
    if (index == 3)
      return plane { vec3i(a.min.x, a.max.y, a.max.z),
                     vec3i(0, 1, 0) };
    if (index == 4)
      return plane { vec3i(a.min.x, a.max.y, a.max.z),
                     vec3i(-1, 0, 0) };
    return plane { a.max, vec3i(1, 0, 0) };
  }

  auto flat_center_of(cref_box a, size_t index) -> vecval {
    return index < 3 ? (a.max[index] + a.min[index]) / 2ll : 0ll;
  }

  auto flat_center_of(cref_triangle a, size_t index) -> vecval {
    if (index < 3) {
      auto p0 = min(min(a[0][index], a[1][index]), a[2][index]);
      auto p1 = max(max(a[0][index], a[1][index]), a[2][index]);
      return (p0 + p1) / 2ll;
    }

    return 0ll;
  }

  auto bounds_of(cref_triangle a) -> box {
    box bounds;
    append(bounds, a);
    return bounds;
  }

  auto bounds_of(cref_quad a) -> box {
    box bounds;
    append(bounds, a);
    return bounds;
  }

  auto bounds_of(cref_polygon a) -> box {
    box bounds;
    append(bounds, a);
    return bounds;
  }

  auto bounds_of(cref_cylinder a) -> box {
    box bounds;
    bounds.min.x = a.base.x - a.radius;
    bounds.min.y = a.base.y - a.radius;
    bounds.min.z = a.base.z;
    bounds.max.x = a.base.x + a.radius;
    bounds.max.y = a.base.y + a.radius;
    bounds.max.z = a.base.z + a.height;
    return bounds;
  }

  auto bounds_of(cref_sphere a) -> box {
    box bounds;
    bounds.min.x = a.center.x - a.radius;
    bounds.min.y = a.center.y - a.radius;
    bounds.min.z = a.center.z - a.radius;
    bounds.max.x = a.center.x + a.radius;
    bounds.max.y = a.center.y + a.radius;
    bounds.max.z = a.center.z + a.radius;
    return bounds;
  }

  auto bounds_of(cref_vtriangle v) -> box {
    box bounds;
    for (size_t i = 1; i < v.size(); i++) append(bounds, v[i]);
    return bounds;
  }

  auto raw_normal(cref_triangle plane) -> vec3i {
    auto a = plane[1] - plane[0];
    auto b = plane[0] - plane[2];
    return math::cross(a, b);
  }

  auto raw_normal(cref_vec3i p0, cref_vec3i p1, cref_vec3i p2)
      -> vec3i {
    auto a = p1 - p0;
    auto b = p0 - p2;
    return math::cross(a, b);
  }

  auto orientation(cref_triangle plane, cref_vec3i point)
      -> vecval {
    auto n = raw_normal(plane);
    auto r = point - plane[0];
    return math::dot(n, r);
  }

  auto square_distance(cref_triangle plane, cref_vec3i point)
      -> vecval {
    auto n = raw_normal(plane);
    auto l = math::square_length(n);

    if (l != 0) {
      auto r = point - plane[0];
      auto d = math::dot(n, r);
      auto x = (d * d + l / 2) / l;
      return d < 0 ? -x : x;
    }

    return safe_limit;
  }

  auto square_distance(cref_ray a, cref_vec3i point) -> vecval {
    auto r = point - a.base;

    if (abs(r.x) + abs(r.y) + abs(r.z) <= epsilon) {
      return 0;
    }

    auto u = math::cross(a.direction, r);
    ray  b = { point, u };

    return square_distance(a, b);
  }

  auto square_distance(cref_ray a, cref_ray b) -> vecval {
    if (auto l0 = math::length(a.direction); l0 != 0)
      if (auto l1 = math::length(b.direction); l1 != 0) {
        vec3i c = math::cross(a.direction, b.direction);
        vec3i n = c / (l0 * l1);
        vec3i r = b.base - a.base;
        vec3i l = n * math::dot(r, n);
        return math::square_length(l);
      }

    return -safe_limit;
  }

  auto abs_square_distance(cref_triangle plane, cref_vec3i point)
      -> vecval {
    auto n = raw_normal(plane);
    auto l = math::square_length(n);

    if (l != 0) {
      auto r = point - plane[0];
      auto d = math::dot(n, r);
      return (d * d + l / 2) / l;
    }

    return safe_limit;
  }

  auto contains_flat(cref_triangle a, cref_vec3i point) -> bool {
    for (size_t i = 0; i < 3; i++) {
      auto  i1 = (i == 2 ? 0 : i + 1);
      auto  i2 = (i == 0 ? 2 : i - 1);
      vec3i v0 = a[i1] - a[i];
      vec3i v1 = a[i2] - a[i];
      vec3i p  = point - a[i];
      vec3i r0 = math::cross(v0, v1);
      vec3i r1 = math::cross(v0, p);
      if (math::dot(r0, r1) + epsilon <= 0)
        return false;
    }

    return true;
  }

  auto contains_flat(cref_quad a, cref_vec3i point) -> bool {
    for (size_t i = 0; i < 4; i++) {
      auto  i1 = (i == 2 ? 0 : i + 1);
      auto  i2 = (i == 0 ? 2 : i - 1);
      vec3i v0 = a[i1] - a[i];
      vec3i v1 = a[i2] - a[i];
      vec3i p  = point - a[i];
      vec3i r0 = math::cross(v0, v1);
      vec3i r1 = math::cross(v0, p);
      if (math::dot(r0, r1) + epsilon <= 0)
        return false;
    }

    return true;
  }

  auto contains_flat(cref_polygon a, cref_vec3i point) -> bool {
    for (size_t i = 0; i < a.size(); i++) {
      auto  i1 = (i == 2 ? 0 : i + 1);
      auto  i2 = (i == 0 ? 2 : i - 1);
      vec3i v0 = a[i1] - a[i];
      vec3i v1 = a[i2] - a[i];
      vec3i p  = point - a[i];
      vec3i r0 = math::cross(v0, v1);
      vec3i r1 = math::cross(v0, p);
      if (math::dot(r0, r1) + epsilon <= 0)
        return false;
    }

    return true;
  }

  auto contains_flat(cref_cylinder a, cref_vec3i point) -> bool {
    if (auto dx = abs(point.x - a.base.x);
        dx - epsilon <= a.radius)
      if (auto dy = abs(point.y - a.base.y);
          dy - epsilon <= a.radius) {
        auto rr = dx * dx + dy * dy;
        return rr - epsilon <= a.radius * a.radius;
      }
    return false;
  }

  auto contains(cref_box a, cref_vec3i point) -> bool {
    if (a.min.x - epsilon >= point.x)
      return false;
    if (a.min.y - epsilon >= point.y)
      return false;
    if (a.min.z - epsilon >= point.z)
      return false;
    if (a.max.x + epsilon <= point.x)
      return false;
    if (a.max.y + epsilon <= point.y)
      return false;
    if (a.max.z + epsilon <= point.z)
      return false;
    return true;
  }

  auto contains(cref_box a, cref_box b) -> bool {
    if (a.min.x - epsilon >= b.min.x)
      return false;
    if (a.min.y - epsilon >= b.min.y)
      return false;
    if (a.min.z - epsilon >= b.min.z)
      return false;
    if (a.max.x + epsilon <= b.max.x)
      return false;
    if (a.max.y + epsilon <= b.max.y)
      return false;
    if (a.max.z + epsilon <= b.max.z)
      return false;
    return true;
  }

  auto contains(cref_box a, cref_triangle b) -> bool {
    if (!contains(a, b[0]))
      return false;
    if (!contains(a, b[1]))
      return false;
    if (!contains(a, b[2]))
      return false;
    return true;
  }

  auto contains(cref_cylinder a, cref_vec3i point) -> bool {
    return a.base.z - epsilon <= point.z &&
           a.base.z + a.height + epsilon >= point.z &&
           contains_flat(a, point);
  }

  auto contains(cref_sphere a, cref_vec3i point) -> bool {
    if (auto dx = abs(point.x - a.center.x);
        dx - epsilon <= a.radius)
      if (auto dy = abs(point.y - a.center.y);
          dy - epsilon <= a.radius)
        if (auto dz = abs(point.z - a.center.z);
            dz - epsilon <= a.radius) {
          auto r = dx * dx + dy * dy + dz * dz;
          return r - epsilon <= a.radius * a.radius;
        }
    return false;
  }

  auto contains(cref_octree a, cref_vec3i point) -> bool {
    if (!contains(a.bounds, point))
      return false;
    if (a.childs.index() == 0) {
      auto &childs = get<0>(a.childs);
      for (auto &c : childs)
        if (contains(c, point))
          return true;
      return false;
    } else {
      auto &childs = get<1>(a.childs);
      for (auto &tr : childs)
        if (orientation(tr, point) - epsilon >= 0)
          return false;
    }
    return true;
  }

  auto intersects(cref_box a, cref_box b) -> bool {
    if (a.max.x + epsilon <= b.min.x)
      return false;
    if (a.max.y + epsilon <= b.min.y)
      return false;
    if (a.max.z + epsilon <= b.min.z)
      return false;
    if (b.max.x + epsilon <= a.min.x)
      return false;
    if (b.max.y + epsilon <= a.min.y)
      return false;
    if (b.max.z + epsilon <= a.min.z)
      return false;
    return true;
  }

  auto intersects(cref_box a, cref_cylinder b) -> bool {
    auto contains_flat = [](vecval x, vecval y,
                            const cylinder &b) -> bool {
      if (auto dx = abs(b.base.x - x); dx - epsilon <= b.radius)
        if (auto dy = abs(b.base.y - y);
            dy - epsilon <= b.radius) {
          auto r = dx * dx + dy * dy;
          return r - epsilon <= b.radius * b.radius;
        }
      return false;
    };
    if (a.max.z + epsilon <= b.base.z)
      return false;
    if (a.min.z - epsilon >= b.base.z + b.height)
      return false;
    if (contains_flat(a.min.x, a.min.y, b))
      return true;
    if (contains_flat(a.min.x, a.max.y, b))
      return true;
    if (contains_flat(a.max.x, a.min.y, b))
      return true;
    if (contains_flat(a.max.x, a.max.y, b))
      return true;
    return false;
  }

  auto intersects(cref_box a, cref_sphere b) -> bool {
    if (a.max.x + epsilon <= b.center.x - b.radius)
      return false;
    if (a.max.y + epsilon <= b.center.y - b.radius)
      return false;
    if (a.max.z + epsilon <= b.center.z - b.radius)
      return false;
    if (a.min.x - epsilon >= b.center.x + b.radius)
      return false;
    if (a.min.y - epsilon >= b.center.y + b.radius)
      return false;
    if (a.min.z - epsilon >= b.center.z + b.radius)
      return false;
    for (size_t i = 0; i < 8; i++)
      if (contains(b, vertex_of(a, i)))
        return true;
    return false;
  }

  auto intersects(cref_box a, cref_triangle b) -> bool {
    auto bounds = bounds_of(b);
    if (!intersects(a, bounds))
      return false;

    auto v0_ori = orientation(b, vertex_of(a, 0));
    if (abs(v0_ori) <= epsilon)
      return true;

    for (size_t i = 1; i < 8; i++) {
      auto v_ori = orientation(b, vertex_of(a, i));
      if (abs(v_ori) <= epsilon)
        return true;
      if ((v0_ori > 0) != (v_ori > 0))
        return true;
    }

    return false;
  }

  auto intersects(cref_box a, cref_octree b) -> bool {
    if (!intersects(a, b.bounds))
      return false;
    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        if (intersects(a, c))
          return true;
      }
    } else {
      for (auto &tr : get<1>(b.childs)) {
        if (intersects(a, tr))
          return true;
      }
    }
    return false;
  }

  auto intersects(cref_cylinder a, cref_cylinder b) -> bool {
    if (a.base.z - epsilon >= b.base.z + b.height)
      return false;
    if (a.base.z + a.height + epsilon <= b.base.z)
      return false;
    auto d = a.radius + b.radius;
    if (auto dx = abs(a.base.x - b.base.x); dx - epsilon <= d)
      if (auto dy = abs(a.base.y - b.base.y); dy - epsilon <= d) {
        auto r = dx * dx + dy * dy;
        return r - epsilon <= d;
      }
    return false;
  }

  auto intersects(cref_cylinder a, cref_sphere b) -> bool {
    if (a.base.z - epsilon >= b.center.z + b.radius)
      return false;
    if (a.base.z + a.height + epsilon <= b.center.z - b.radius)
      return false;

    vecval dx, dy;
    auto   d = a.radius + b.radius;
    if (dx = abs(a.base.x - b.center.x); dx - epsilon >= d)
      return false;
    if (dy = abs(a.base.y - b.center.y); dy - epsilon >= d)
      return false;
    auto r = dx * dx + dy * dy;
    if (r - epsilon >= d)
      return false;

    auto l = static_cast<vecval>(
        sqrtl(static_cast<long double>(r)) -
        static_cast<long double>(a.radius) + .5);
    auto h = a.radius * a.radius - l * l;
    if (a.base.z - epsilon >= b.center.z + h)
      return false;
    if (a.base.z + a.height + epsilon <= b.center.z - h)
      return false;
    return true;
  }

  auto intersects(cref_cylinder a, cref_triangle b) -> bool {
    if (contains(a, b[0]))
      return true;
    if (contains(a, b[1]))
      return true;
    if (contains(a, b[2]))
      return true;

    if (a.base.z + epsilon >= b[0].z)
      if (a.base.z + epsilon >= b[1].z)
        if (a.base.z + epsilon >= b[2].z)
          return false;

    if (a.base.z + a.height - epsilon <= b[0].z)
      if (a.base.z + a.height - epsilon <= b[1].z)
        if (a.base.z + a.height - epsilon <= b[2].z)
          return false;

    ray  stem { a.base, vec3i(0, 0, 1) };
    auto d = a.radius * a.radius;

    if (square_distance(stem, ray { b[0], b[1] - b[0] }) -
            epsilon <=
        d)
      return true;
    if (square_distance(stem, ray { b[1], b[2] - b[1] }) -
            epsilon <=
        d)
      return true;
    if (square_distance(stem, ray { b[2], b[0] - b[2] }) -
            epsilon <=
        d)
      return true;

    return intersection(stem, plane_of(b)) + epsilon >= 0;
  }

  auto intersects(cref_cylinder a, cref_octree b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        if (intersects(a, c)) {
          return true;
        }
      }
    } else {
      for (auto &tr : get<1>(b.childs)) {
        if (intersects(a, tr)) {
          return true;
        }
      }
    }
    return false;
  }

  auto intersects(cref_sphere a, cref_sphere b) -> bool {
    auto d = math::square_length(a.center - b.center);
    return d - epsilon <= a.radius + b.radius;
  }

  auto intersects(cref_sphere a, cref_triangle b) -> bool {
    if (contains(a, b[0]))
      return true;
    if (contains(a, b[1]))
      return true;
    if (contains(a, b[2]))
      return true;
    return abs_square_distance(b, a.center) - epsilon <=
           a.radius * a.radius;
  }

  auto intersects(cref_sphere a, cref_octree b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        if (intersects(a, c)) {
          return true;
        }
      }
    } else {
      for (auto &tr : get<1>(b.childs)) {
        if (intersects(a, tr)) {
          return true;
        }
      }
    }
    return false;
  }

  auto intersects(cref_triangle a, cref_triangle b) -> bool {
    auto ori0 = orientation(b, a[0]);
    auto ori1 = orientation(b, a[1]);
    auto ori2 = orientation(b, a[2]);

    if (abs(ori0) >= epsilon && abs(ori1) >= epsilon &&
        abs(ori2) >= epsilon) {
      if ((ori0 < 0) == (ori1 < 0))
        if ((ori0 < 0) == (ori2 < 0))
          return false;
    }

    if (contains_flat(a, b[0]))
      return true;
    if (contains_flat(a, b[1]))
      return true;
    if (contains_flat(a, b[2]))
      return true;
    if (contains_flat(b, a[0]))
      return true;
    if (contains_flat(b, a[1]))
      return true;
    if (contains_flat(b, a[2]))
      return true;

    return false;
  }

  auto intersects(cref_triangle a, cref_octree b) -> bool {
    if (!intersects(b.bounds, a))
      return false;
    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        if (intersects(a, c)) {
          return true;
        }
      }
    } else {
      for (auto &tr : get<1>(b.childs)) {
        if (intersects(a, tr)) {
          return true;
        }
      }
    }
    return false;
  }

  auto intersects(cref_octree a, cref_octree b) -> bool {
    if (!intersects(a.bounds, b.bounds))
      return false;
    if (a.childs.index() == 0) {
      auto &childs = get<0>(a.childs);
      for (auto &c : childs)
        if (intersects(c, b))
          return true;
    } else if (b.childs.index() == 0) {
      auto &childs = get<0>(b.childs);
      for (auto &c : childs)
        if (intersects(a, c))
          return true;
    } else {
      auto &v0 = get<1>(a.childs);
      auto &v1 = get<1>(b.childs);

      for (auto &tr0 : v0) {
        if (intersects(b.bounds, tr0)) {
          for (auto &tr1 : v1)
            if (intersects(tr0, tr1))
              return true;
        }
      }
    }
    return false;
  }

  auto intersects(cref_ray a, cref_plane b) -> bool {
    auto r     = b.base - a.base;
    auto r_cos = math::dot(r, b.normal);
    auto n     = r_cos < 0 ? b.normal : -b.normal;
    auto v     = n * math::dot(n, a.direction);

    return v.x * v.x >= epsilon || v.y * v.y >= epsilon ||
           v.z * v.z >= epsilon;
  }

  auto intersects(cref_ray a, cref_triangle b) -> bool {
    vecval t = intersection(a, plane_of(b));
    return t + epsilon >= 0 && contains_flat(b, point_of(a, t));
  }

  auto intersects(cref_ray a, cref_quad b) -> bool {
    auto t = intersection(a, plane_of(b));
    return t + epsilon >= 0 && contains_flat(b, point_of(a, t));
  }

  auto intersects(cref_ray a, cref_polygon b) -> bool {
    auto t = intersection(a, plane_of(b));
    return t + epsilon >= 0 && contains_flat(b, point_of(a, t));
  }

  auto intersects(cref_ray a, cref_box b) -> bool {
    if (contains(b, a.base))
      return true;

    auto t0 = min(intersection(a, plane_of(b, 0)),
                  intersection(a, plane_of(b, 1)));
    auto t1 = min(intersection(a, plane_of(b, 2)),
                  intersection(a, plane_of(b, 3)));
    auto t2 = min(intersection(a, plane_of(b, 4)),
                  intersection(a, plane_of(b, 5)));

    auto t = max(t0, max(t1, t2));
    auto p = point_of(a, t);

    return contains(b, p);
  }

  auto intersects(cref_ray ra, cref_cylinder cyl) -> bool {
    auto rr = cyl.radius * cyl.radius;

    if (square_distance(ra, ray { cyl.base, vec3i { 0, 0, 1 } }) +
            epsilon >=
        rr) {
      return false;
    }

    auto t0 = intersection(
        ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    if (t0 + epsilon >= 0 &&
        contains_flat(cyl, point_of(ra, t0))) {
      return true;
    }

    auto t1 = intersection(
        ra, plane { vec3i { cyl.base.x, cyl.base.y,
                            cyl.base.z + cyl.height },
                    vec3i { 0, 0, 1 } });

    if (t1 + epsilon >= 0 &&
        contains_flat(cyl, point_of(ra, t1))) {
      return true;
    }

    return false;
  }

  auto intersects(cref_ray ra, cref_sphere sph) -> bool {
    return square_distance(ra, sph.center) - epsilon <=
           sph.radius * sph.radius;
  }

  auto intersects(cref_ray a, cref_octree b) -> bool {
    if (!intersects(a, b.bounds)) {
      return false;
    }

    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        if (intersects(a, c)) {
          return true;
        }
      }
    } else {
      for (auto &tr : get<1>(b.childs)) {
        if (intersects(a, tr)) {
          return true;
        }
      }
    }

    return false;
  }

  auto intersection(cref_ray a, cref_plane b) -> vecval {
    auto r     = b.base - a.base;
    auto r_cos = math::dot(r, b.normal);
    auto n     = r_cos < 0 ? b.normal : -b.normal;
    auto k     = math::length(n * math::dot(n, a.direction));

    if (k != 0) {
      auto h = math::length(n * r_cos);
      auto d = math::length(a.direction);
      return (h * d + k / 2) / k;
    }

    return -safe_limit;
  }

  auto intersection(cref_ray a, cref_triangle b) -> vecval {
    auto t = intersection(a, plane_of(b));

    if (t > -infinity && !contains_flat(b, point_of(a, t))) {
      t = -safe_limit;
    }

    return t;
  }

  auto intersection(cref_ray a, cref_quad b) -> vecval {
    auto t = intersection(a, plane_of(b));

    if (t > -infinity && !contains_flat(b, point_of(a, t))) {
      t = -safe_limit;
    }

    return t;
  }

  auto intersection(cref_ray a, cref_polygon b) -> vecval {
    auto t = intersection(a, plane_of(b));

    if (t > -infinity && !contains_flat(b, point_of(a, t))) {
      t = -safe_limit;
    }

    return t;
  }

  auto intersection(cref_ray a, cref_box b) -> vecval {
    if (contains(b, a.base)) {
      auto t0 = max(intersection(a, plane_of(b, 0)),
                    intersection(a, plane_of(b, 1)));
      auto t1 = max(intersection(a, plane_of(b, 2)),
                    intersection(a, plane_of(b, 3)));
      auto t2 = max(intersection(a, plane_of(b, 4)),
                    intersection(a, plane_of(b, 5)));

      return min(t0, min(t1, t2));
    }

    auto t0 = min(intersection(a, plane_of(b, 0)),
                  intersection(a, plane_of(b, 1)));
    auto t1 = min(intersection(a, plane_of(b, 2)),
                  intersection(a, plane_of(b, 3)));
    auto t2 = min(intersection(a, plane_of(b, 4)),
                  intersection(a, plane_of(b, 5)));

    auto t = max(t0, max(t1, t2));
    auto p = point_of(a, t);

    return contains(b, p) ? t : -safe_limit;
  }

  /*  Minimal nonnegative equation root, or nearest to
   *  zero root, if there is no nonnegative roots.
   *
   *  The equation:
   *      a x^2 + b x + c = 0
   */
  static auto quadratic_solution(vecval a, vecval b, vecval c)
      -> vecval {
    if (a == 0) {
      /*  Linear solution.
       */

      if (b == 0) {
        /*  No intersection.
         */

        return -safe_limit;
      }

      return -c / b;
    }

    /*  Quadratic solution.
     */

    auto d = b * b - 4 * a * c;

    if (d < 0) {
      return -safe_limit;
    }

    auto delta = static_cast<vecval>(
        sqrtl(static_cast<long double>(d)));

    return (b <= 0 && -b >= delta ? -b - delta : -b + delta) /
           (2 * a);
  }

  auto intersection(cref_ray ra, cref_cylinder cyl) -> vecval {
    auto rr = cyl.radius * cyl.radius;

    if (square_distance(ra, ray { cyl.base, vec3i { 0, 0, 1 } }) +
            epsilon >=
        rr) {
      return -safe_limit;
    }

    auto t0 = intersection(
        ra, plane { cyl.base, vec3i { 0, 0, 1 } });

    auto t1 = intersection(
        ra, plane { vec3i { cyl.base.x, cyl.base.y,
                            cyl.base.z + cyl.height },
                    vec3i { 0, 0, 1 } });

    auto p0 = point_of(ra, t0);
    auto p1 = point_of(ra, t1);

    auto contains_p0 = contains(cyl, p0);
    auto contains_p1 = contains(cyl, p1);

    if (t0 >= 0 && contains_p0) {
      return t0;
    }

    if (t1 >= 0 && contains_p1) {
      return t1;
    }

    auto cx = cyl.base.x;
    auto cy = cyl.base.y;

    auto x0 = ra.base.x;
    auto y0 = ra.base.y;

    auto vx = ra.direction.x;
    auto vy = ra.direction.y;

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

    auto dx = x0 - cx;
    auto dy = y0 - cy;

    /*  Intersection condition:
     *
     *      (vx t + dx)^2 + (vy t + dy)^2 - r^2 = 0
     *
     *      (vx^2 + vy^2) t^2 + 2 (vx dx + vy dy) t + (dx^2 +
     * dy^2 - r^2) = 0
     */

    auto t = quadratic_solution(vx * vx + vy * vy,
                                2 * (vx * dx + vy * dy),
                                dx * dx + dy * dy - rr);

    if (t > -safe_limit) {
      auto z = ra.base.z + ra.direction.z * t;

      if (z >= cyl.base.z - epsilon ||
          z <= cyl.base.z + cyl.height + epsilon) {
        return t;
      }

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

  auto intersection(cref_ray ra, cref_sphere sph) -> vecval {
    auto rr = sph.radius * sph.radius;

    if (square_distance(ra, sph.center) + epsilon >= rr) {
      return -safe_limit;
    }

    auto cx = sph.center.x;
    auto cy = sph.center.y;
    auto cz = sph.center.z;

    auto x0 = ra.base.x;
    auto y0 = ra.base.y;
    auto z0 = ra.base.z;

    auto vx = ra.direction.x;
    auto vy = ra.direction.y;
    auto vz = ra.direction.z;

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
     *      (vx t + x0 - cx)^2 + (vy t + y0 - cy)^2 + (vz t + z0
     * - cz)^2 - r^2 = 0
     */

    auto dx = x0 - cx;
    auto dy = y0 - cy;
    auto dz = z0 - cz;

    /*  Intersection condition:
     *
     *      (vx t + dx)^2 + (vy t + dy)^2 + (vz t + dz)^2 - r^2
     * = 0
     *
     *      (vx^2 + vy^2 + vz^2) t^2 + 2 (vx dx + vy dy + vz dz)
     * t + (dx^2 + dy^2 + dz^2 - r^2) = 0
     */

    return quadratic_solution(vx * vx + vy * vy + vz * vz,
                              2 * (vx * dx + vy * dy + vz * vz),
                              dx * dx + dy * dy + dz * dz - rr);
  }

  auto intersection(cref_ray a, cref_octree b) -> vecval {
    if (!intersects(a, b.bounds)) {
      return -safe_limit;
    }

    if (b.childs.index() == 0) {
      for (auto &c : get<0>(b.childs)) {
        auto t = intersection(a, c);

        if (t + epsilon >= 0) {
          return t;
        }
      }
    } else {
      auto result = safe_limit;
      for (auto &tr : get<1>(b.childs)) {
        auto t = intersection(a, tr);

        if (t + epsilon >= 0 && t < result) {
          result = t;
        }
      }
      if (result < infinity) {
        return result;
      }
    }

    return -safe_limit;
  }
}
