/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "box.h"

#include "polygon.h"
#include "quad.h"
#include "triangle.h"
#include <numeric>

namespace laplace::engine::tridi {
  using std::midpoint;

  auto box::is_empty() const noexcept -> bool {
    return min.x() > max.x() || min.y() > max.y() ||
           min.z() > max.z();
  }

  auto box::is_equals(box const &other) const noexcept -> bool {
    if (is_empty() && other.is_empty())
      return true;
    return min == other.min && max == other.max;
  }

  auto box::get_vertex(sl::index n) const noexcept -> vec3i {
    switch (n) {
      case 0: return min;
      case 1: return { min.x(), min.y(), max.z() };
      case 2: return { min.x(), max.y(), min.z() };
      case 3: return { min.x(), max.y(), max.z() };
      case 4: return { max.x(), min.y(), min.z() };
      case 5: return { max.x(), min.y(), max.z() };
      case 6: return { max.x(), max.y(), min.z() };
    }
    return max;
  }

  auto box::get_plane(sl::index n) const noexcept -> plane {
    switch (n) {
      case 0: return { min, { 0, 0, -1 } };
      case 1: return { { min.x(), min.y(), max.z() }, { 0, 0, 1 } };
      case 2: return { { min.x(), min.y(), max.z() }, { 0, -1, 0 } };
      case 3: return { { min.x(), max.y(), max.z() }, { 0, 1, 0 } };
      case 4: return { { min.x(), max.y(), max.z() }, { -1, 0, 0 } };
    }
    return { max, { 1, 0, 0 } };
  }

  auto box::get_flat_center(sl::index axis) const noexcept -> intval {
    if (axis >= 0 && axis < 3)
      return midpoint(min[axis], max[axis]);
    return {};
  }

  auto box::get_center() const noexcept -> vec3i {
    return { get_flat_center(0), get_flat_center(1),
             get_flat_center(2) };
  }

  auto box::append(vec3i const &point) const noexcept -> box {
    if (is_empty())
      return { .min = point, .max = point };

    return { .min = { std::min(min.x(), point.x()),
                      std::min(min.y(), point.y()),
                      std::min(min.z(), point.z()) },
             .max = { std::max(max.x(), point.x()),
                      std::max(max.y(), point.y()),
                      std::max(max.z(), point.z()) } };
  }

  auto box::append(triangle const &a) const noexcept -> box {
    return append(a.vertices[0])
        .append(a.vertices[1])
        .append(a.vertices[2]);
  }

  auto box::append(quad const &a) const noexcept -> box {
    return append(a.vertices[0])
        .append(a.vertices[1])
        .append(a.vertices[2])
        .append(a.vertices[3]);
  }

  auto box::append(polygon const &a) const noexcept -> box {
    auto x = *this;
    for (auto const &v : a.vertices) x = x.append(v);
    return x;
  }

  auto box::contains(vec3i const &point) const noexcept -> bool {
    return min.x() <= point.x() && min.y() <= point.y() &&
           min.z() <= point.z() && max.x() >= point.x() &&
           max.y() >= point.y() && max.z() >= point.z();
  }

  auto box::contains(box const &b) const noexcept -> bool {
    return min.x() <= b.min.x() && min.y() <= b.min.y() &&
           min.z() <= b.min.z() && max.x() >= b.max.x() &&
           max.y() >= b.max.y() && max.z() >= b.max.z();
  }

  auto box::contains(triangle const &tri) const noexcept -> bool {
    return contains(tri.vertices[0]) && contains(tri.vertices[1]) &&
           contains(tri.vertices[2]);
  }

  auto operator==(box const &a, box const &b) noexcept -> bool {
    return a.is_equals(b);
  }

  auto operator!=(box const &a, box const &b) noexcept -> bool {
    return !a.is_equals(b);
  }
}
