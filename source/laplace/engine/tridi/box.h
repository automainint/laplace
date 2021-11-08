/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_box_h
#define laplace_engine_tridi_box_h

#include "plane.h"

namespace laplace::engine::tridi {
  struct triangle;
  struct quad;
  struct polygon;

  struct box {
    vec3i min = { 1, 1, 1 };
    vec3i max = { -1, -1, -1 };

    [[nodiscard]] auto is_empty() const noexcept -> bool;
    [[nodiscard]] auto is_equals(box const &other) const noexcept
        -> bool;
    [[nodiscard]] auto get_vertex(sl::index n) const noexcept
        -> vec3i;
    [[nodiscard]] auto get_plane(sl::index n) const noexcept -> plane;
    [[nodiscard]] auto get_flat_center(sl::index axis) const noexcept
        -> intval;
    [[nodiscard]] auto get_center() const noexcept -> vec3i;
    [[nodiscard]] auto append(vec3i const &point) const noexcept
        -> box;
    [[nodiscard]] auto append(triangle const &b) const noexcept
        -> box;
    [[nodiscard]] auto append(quad const &b) const noexcept -> box;
    [[nodiscard]] auto append(polygon const &b) const noexcept -> box;
    [[nodiscard]] auto contains(vec3i const &point) const noexcept
        -> bool;
    [[nodiscard]] auto contains(box const &b) const noexcept -> bool;
    [[nodiscard]] auto contains(triangle const &tri) const noexcept
        -> bool;
  };

  [[nodiscard]] auto operator==(box const &a, box const &b) noexcept
      -> bool;
  [[nodiscard]] auto operator!=(box const &a, box const &b) noexcept
      -> bool;
}

#endif
