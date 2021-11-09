/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_tridi_intersection_h
#define laplace_engine_tridi_intersection_h

#include "../defs.h"

namespace laplace::engine::tridi {
  struct box;
  struct plane;
  struct triangle;
  struct quad;
  struct polygon;
  struct cylinder;
  struct sphere;
  struct ray;
  struct segment;

  [[nodiscard]] auto intersects(box const &a, box const &b) noexcept
      -> bool;
  [[nodiscard]] auto intersects(box const      &bo,
                                cylinder const &cyl) noexcept -> bool;
  [[nodiscard]] auto intersects(box const    &bo,
                                sphere const &sph) noexcept -> bool;
  [[nodiscard]] auto intersects(box const      &bo,
                                triangle const &tri) noexcept -> bool;
  [[nodiscard]] auto intersects(cylinder const &a,
                                cylinder const &b) noexcept -> bool;
  [[nodiscard]] auto intersects(cylinder const &cyl,
                                sphere const   &sph) noexcept -> bool;
  [[nodiscard]] auto intersects(sphere const &a,
                                sphere const &b) noexcept -> bool;
  [[nodiscard]] auto intersects(triangle const &tri,
                                segment const  &seg) noexcept -> bool;
  [[nodiscard]] auto intersects(ray const      &ra,
                                triangle const &tri) noexcept -> bool;
  [[nodiscard]] auto intersects(ray const &ra, box const &bo) noexcept
      -> bool;
  [[nodiscard]] auto intersection(ray const   &ra,
                                  plane const &pla) noexcept
      -> intval;
}

#endif
