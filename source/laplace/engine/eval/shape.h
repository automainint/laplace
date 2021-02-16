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

#ifndef __laplace__engine_eval_shape__
#define __laplace__engine_eval_shape__

#include "geometry.h"

namespace laplace::engine::eval {
  class shape {
  public:
    using cref_shape = const shape &;

    void granulate(cref_vtriangle triangles, int64_t delta);
    auto contains(cref_vec3i point) const -> bool;
    auto get_bounds() const -> box;

    void set_empty();
    void set_box(cref_box value);
    void set_cylinder(cref_cylinder value);
    void set_sphere(cref_sphere value);
    void set_octree(cref_octree value);

    auto reset_box() -> ref_box;
    auto reset_cylinder() -> ref_cylinder;
    auto reset_sphere() -> ref_sphere;
    auto reset_octree() -> ref_octree;

    auto is_empty() const -> bool;
    auto is_box() const -> bool;
    auto is_cylinder() const -> bool;
    auto is_sphere() const -> bool;
    auto is_octree() const -> bool;

    auto get_box() const -> cref_box;
    auto get_cylinder() const -> cref_cylinder;
    auto get_sphere() const -> cref_sphere;
    auto get_octree() const -> cref_octree;

  private:
    static constexpr size_t n_box      = 1;
    static constexpr size_t n_cylinder = 2;
    static constexpr size_t n_sphere   = 3;
    static constexpr size_t n_octree   = 4;

    std::variant<std::monostate, box, cylinder, sphere, octree> m_data;
  };

  using cref_shape = shape::cref_shape;

  auto intersects(cref_shape a, cref_shape b) -> bool;
  auto intersects(cref_ray a, cref_shape b) -> bool;
  auto intersection(cref_ray a, cref_shape b) -> int64_t;
}

#endif
