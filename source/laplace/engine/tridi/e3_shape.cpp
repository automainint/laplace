/*  laplace/engine/eval/ee_shape.cpp
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

#include "shape.h"

#include "intersection.h"

namespace laplace::engine::tridi {
  using std::monostate, std::span;

  static void build(octree &tree, intval delta, box const &bounds) {
    tree.bounds = bounds;

    intval const d[3] = { bounds.max.x() - bounds.min.x(),
                          bounds.max.y() - bounds.min.y(),
                          bounds.max.z() - bounds.min.z() };

    bool const f[3] = { d[0] > delta, d[1] > delta, d[2] > delta };

    intval const s[3] = { f[0] ? d[0] / 2 : d[0],
                          f[1] ? d[1] / 2 : d[1],
                          f[2] ? d[2] / 2 : d[2] };

    const auto n = (f[0] ? 2u : 1u) * (f[1] ? 2u : 1u) *
                   (f[2] ? 2u : 1u);

    if (n == 1) {
      tree.children = sl::vector<triangle> {};
      return;
    }

    tree.children = sl::vector<octree>(n);
    auto &v     = get<0>(tree.children);

    sl::index n_child = 0;

    for (sl::index i = 0; i < 2; i++) {
      if (i == 1 && !f[0])
        continue;

      for (sl::index j = 0; j < 2; j++) {
        if (j == 1 && !f[1])
          continue;

        for (sl::index k = 0; k < 2; k++) {
          if (k == 1 && !f[2])
            continue;

          build(
              v[n_child], delta,
              box {
                  { i == 0 ? bounds.min.x() : bounds.min.x() + s[0],
                    j == 0 ? bounds.min.y() : bounds.min.y() + s[1],
                    k == 0 ? bounds.min.z() : bounds.min.z() + s[2] },
                  { i == 0 && f[0] ? bounds.min.x() + s[0]
                                   : bounds.max.x(),
                    j == 0 && f[1] ? bounds.min.y() + s[1]
                                   : bounds.max.y(),
                    k == 0 && f[2] ? bounds.min.z() + s[2]
                                   : bounds.max.z() } });

          n_child++;
        }
      }
    }
  }

  static void put(octree &tree, triangle const &tri) {
    if (!intersects(tree.bounds, tri))
      return;
    if (tree.children.index() == 0)
      for (auto &child : get<0>(tree.children)) put(child, tri);
    else
      get<1>(tree.children).emplace_back(tri);
  }

  static void cleanup(octree &tree) {
    auto is_empty = [](octree &tree) -> bool {
      if (tree.children.index() == 0)
        return get<0>(tree.children).empty();
      return get<1>(tree.children).empty();
    };

    if (tree.children.index() != 0)
      return;

    auto &v = get<0>(tree.children);

    for (sl::index i = 0; i < v.size();) {
      cleanup(v[i]);

      if (is_empty(v[i]))
        v.erase(v.begin() + i);
      else
        i++;
    }
  }

  void shape::granulate(span<triangle const> triangles,
                        int64_t              delta) {
    build(reset_octree(), delta, bounds_of(triangles));
    for (auto const &tri : triangles) put(get<n_octree>(m_data), tri);
    cleanup(get<n_octree>(m_data));
  }

  auto shape::contains(vec3i const &point) const -> bool {
    return is_box()        ? get<n_box>(m_data).contains(point)
           : is_cylinder() ? get<n_cylinder>(m_data).contains(point)
           : is_sphere()
               ? get<n_sphere>(m_data).contains(point)
               : is_octree() && get<n_octree>(m_data).contains(point);
  }

  auto shape::get_bounds() const -> box {
    return is_box()        ? get<n_box>(m_data)
           : is_cylinder() ? get<n_cylinder>(m_data).get_bounds()
           : is_sphere()   ? get<n_sphere>(m_data).get_bounds()
           : is_octree()   ? get<n_octree>(m_data).bounds
                           : box {};
  }

  void shape::set_empty() {
    m_data = monostate();
  }

  void shape::set_box(box const &value) {
    m_data = value;
  }

  void shape::set_cylinder(cylinder const &value) {
    m_data = value;
  }

  void shape::set_sphere(sphere const &value) {
    m_data = value;
  }

  void shape::set_octree(octree const &value) {
    m_data = value;
  }

  auto shape::reset_box() -> box & {
    if (m_data.index() != n_box)
      m_data = box {};
    return get<n_box>(m_data);
  }

  auto shape::reset_cylinder() -> cylinder & {
    if (m_data.index() != n_cylinder)
      m_data = cylinder {};
    return get<n_cylinder>(m_data);
  }

  auto shape::reset_sphere() -> sphere & {
    if (m_data.index() != n_sphere)
      m_data = sphere();
    return get<n_sphere>(m_data);
  }

  auto shape::reset_octree() -> octree & {
    if (m_data.index() != n_octree)
      m_data = octree();
    return get<n_octree>(m_data);
  }

  auto shape::is_empty() const -> bool {
    return m_data.index() == n_empty;
  }

  auto shape::is_box() const -> bool {
    return m_data.index() == n_box;
  }

  auto shape::is_cylinder() const -> bool {
    return m_data.index() == n_cylinder;
  }

  auto shape::is_sphere() const -> bool {
    return m_data.index() == n_sphere;
  }

  auto shape::is_octree() const -> bool {
    return m_data.index() == n_octree;
  }

  auto shape::get_box() const -> box const & {
    if (m_data.index() != n_box) {
      static constexpr box nil;
      return nil;
    }

    return get<n_box>(m_data);
  }

  auto shape::get_cylinder() const -> cylinder const & {
    if (m_data.index() != n_cylinder) {
      static constexpr cylinder nil;
      return nil;
    }

    return get<n_cylinder>(m_data);
  }

  auto shape::get_sphere() const -> sphere const & {
    if (m_data.index() != n_sphere) {
      static constexpr sphere nil;
      return nil;
    }

    return get<n_sphere>(m_data);
  }

  auto shape::get_octree() const -> octree const & {
    if (m_data.index() != n_octree) {
      static const octree nil;
      return nil;
    }

    return get<n_octree>(m_data);
  }
}
