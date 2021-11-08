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

  auto intersects(box const &a, box const &b) -> bool;
  auto intersects(box const &a, cylinder const &b) -> bool;
  auto intersects(box const &a, sphere const &b) -> bool;
  auto intersects(box const &a, triangle const &b) -> bool;
  auto intersects(cylinder const &a, cylinder const &b) -> bool;
  auto intersects(cylinder const &a, sphere const &b) -> bool;
  auto intersects(cylinder const &a, triangle const &b) -> bool;
  auto intersects(sphere const &a, sphere const &b) -> bool;
  auto intersects(sphere const &a, triangle const &b) -> bool;
  auto intersects(triangle const &a, triangle const &b) -> bool;

  auto intersects(ray const &a, plane const &b) -> bool;
  auto intersects(ray const &a, triangle const &b) -> bool;
  auto intersects(ray const &a, quad const &b) -> bool;
  auto intersects(ray const &a, polygon const &b) -> bool;

  auto intersects(ray const &a, box const &b) -> bool;
  auto intersects(ray const &a, cylinder const &b) -> bool;
  auto intersects(ray const &a, sphere const &b) -> bool;

  auto intersection(ray const &a, plane const &b) -> intval;
  auto intersection(ray const &a, triangle const &b) -> intval;
  auto intersection(ray const &a, quad const &b) -> intval;
  auto intersection(ray const &a, polygon const &b) -> intval;

  auto intersection(ray const &a, box const &b) -> intval;
  auto intersection(ray const &a, cylinder const &b) -> intval;
  auto intersection(ray const &a, sphere const &b) -> intval;
}

#endif
