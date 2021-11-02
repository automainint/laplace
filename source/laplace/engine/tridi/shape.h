/*  laplace/engine/eval/shape.h
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

#ifndef laplace_engine_tridi_shape_h
#define laplace_engine_tridi_shape_h

#include "box.h"
#include "cylinder.h"
#include "octree.h"
#include "sphere.h"
#include "triangle.h"

namespace laplace::engine::tridi {
  class shape {
  public:
    using cref_shape = const shape &;

    void granulate(std::span<triangle const> triangles, intval delta);
    [[nodiscard]] auto contains(vec3i const &point) const -> bool;
    [[nodiscard]] auto get_bounds() const -> box;

    void set_empty();
    void set_box(box const &value);
    void set_cylinder(cylinder const &value);
    void set_sphere(sphere const &value);
    void set_octree(octree const &value);

    auto reset_box() -> box &;
    auto reset_cylinder() -> cylinder &;
    auto reset_sphere() -> sphere &;
    auto reset_octree() -> octree &;

    [[nodiscard]] auto is_empty() const -> bool;
    [[nodiscard]] auto is_box() const -> bool;
    [[nodiscard]] auto is_cylinder() const -> bool;
    [[nodiscard]] auto is_sphere() const -> bool;
    [[nodiscard]] auto is_octree() const -> bool;

    [[nodiscard]] auto get_box() const -> box const &;
    [[nodiscard]] auto get_cylinder() const -> cylinder const &;
    [[nodiscard]] auto get_sphere() const -> sphere const &;
    [[nodiscard]] auto get_octree() const -> octree const &;

  private:
    enum _index : sl::index {
      n_empty    = 0,
      n_box      = 1,
      n_cylinder = 2,
      n_sphere   = 3,
      n_octree   = 4
    };

    std::variant<std::monostate, box, cylinder, sphere, octree>
        m_data;
  };
}

#endif
