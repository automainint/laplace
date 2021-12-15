/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/tridi/box.h"
#include "../../laplace/engine/tridi/cylinder.h"
#include "../../laplace/engine/tridi/intersection.h"
#include "../../laplace/engine/tridi/sphere.h"
#include "../../laplace/engine/tridi/triangle.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::tridi::intersects, engine::tridi::box,
      engine::tridi::cylinder, engine::tridi::sphere,
      engine::tridi::triangle, engine::tridi::ray, engine::vec3i;

  TEST(engine, tridi_intersects_box) {
    auto foo = box { .min = { -1, -1, -1 }, .max = { 1, 2, 3 } };
    auto bar = box { .min = { 1, 2, 3 }, .max = { 4, 5, 6 } };
    auto bus = box { .min = { 2, 3, 4 }, .max = { 3, 4, 5 } };

    EXPECT_TRUE(intersects(foo, bar));
    EXPECT_TRUE(intersects(bar, foo));
    EXPECT_TRUE(intersects(bar, bus));
    EXPECT_TRUE(intersects(bus, bar));
    EXPECT_FALSE(intersects(foo, bus));
    EXPECT_FALSE(intersects(bus, foo));
  }

  TEST(engine, tridi_intersects_box_cylinder_1) {
    auto foo = box { .min = { -1, -1, -2 }, .max = { 1, 1, 2 } };

    auto bar = cylinder { .base   = { 0, 0, 2 },
                          .radius = 1,
                          .height = 1 };

    auto bus = cylinder { .base   = { 0, 0, 3 },
                          .radius = 1,
                          .height = 1 };

    auto rab = cylinder { .base   = { 0, 0, -4 },
                          .radius = 1,
                          .height = 1 };

    auto oof = cylinder { .base   = { -1, 0, 1 },
                          .radius = 1,
                          .height = 1 };

    EXPECT_TRUE(intersects(foo, bar));
    EXPECT_TRUE(intersects(foo, oof));
    EXPECT_FALSE(intersects(foo, bus));
    EXPECT_FALSE(intersects(foo, rab));
  }

  TEST(engine, tridi_intersects_box_cylinder_2) {
    auto cyl = cylinder { .base   = { 0, 0, 2 },
                          .radius = 1,
                          .height = 1 };

    auto box1 = box { .min = { -2, -1, 2 }, .max = { -1, 1, 3 } };
    auto box2 = box { .min = { 1, -1, 2 }, .max = { 2, 1, 3 } };
    auto box3 = box { .min = { -1, -2, 2 }, .max = { 1, -1, 3 } };
    auto box4 = box { .min = { -1, 1, 2 }, .max = { 1, 2, 3 } };

    EXPECT_TRUE(intersects(box1, cyl));
    EXPECT_TRUE(intersects(box2, cyl));
    EXPECT_TRUE(intersects(box3, cyl));
    EXPECT_TRUE(intersects(box4, cyl));
  }

  TEST(engine, tridi_intersects_box_cylinder_3) {
    auto cyl = cylinder { .base   = { 0, 0, 2 },
                          .radius = 1,
                          .height = 1 };

    auto box1 = box { .min = { -2, -2, 2 }, .max = { -1, -1, 3 } };
    auto box2 = box { .min = { 1, -2, 2 }, .max = { 2, -1, 3 } };
    auto box3 = box { .min = { -2, -2, 2 }, .max = { -1, -1, 3 } };
    auto box4 = box { .min = { -2, 1, 2 }, .max = { -1, 2, 3 } };

    EXPECT_FALSE(intersects(box1, cyl));
    EXPECT_FALSE(intersects(box2, cyl));
    EXPECT_FALSE(intersects(box3, cyl));
    EXPECT_FALSE(intersects(box4, cyl));
  }

  TEST(engine, tridi_intersects_box_cylinder_4) {
    auto cyl = cylinder { .base   = { 0, 0, 2 },
                          .radius = 1,
                          .height = 1 };

    auto box1 = box { .min = { -2, -2, 2 }, .max = { 0, 0, 3 } };
    auto box2 = box { .min = { 0, -2, 2 }, .max = { 2, 0, 3 } };
    auto box3 = box { .min = { -2, 0, 2 }, .max = { 0, 2, 3 } };
    auto box4 = box { .min = { 0, 0, 2 }, .max = { 2, 2, 3 } };

    EXPECT_TRUE(intersects(box1, cyl));
    EXPECT_TRUE(intersects(box2, cyl));
    EXPECT_TRUE(intersects(box3, cyl));
    EXPECT_TRUE(intersects(box4, cyl));
  }

  TEST(engine, tridi_intersects_box_sphere_1) {
    auto sph = sphere { .center = { 0, 0, 4 }, .radius = 2 };

    auto box1 = box { .min = { -3, -2, 2 }, .max = { -1, 2, 6 } };
    auto box2 = box { .min = { 1, -2, 2 }, .max = { 3, 2, 6 } };
    auto box3 = box { .min = { -2, -3, 2 }, .max = { 2, -1, 6 } };
    auto box4 = box { .min = { -2, 1, 2 }, .max = { 2, 3, 6 } };
    auto box5 = box { .min = { -2, -2, 1 }, .max = { 2, 2, 3 } };
    auto box6 = box { .min = { -2, -2, 5 }, .max = { 2, 2, 7 } };

    EXPECT_TRUE(intersects(box1, sph));
    EXPECT_TRUE(intersects(box2, sph));
    EXPECT_TRUE(intersects(box3, sph));
    EXPECT_TRUE(intersects(box4, sph));
    EXPECT_TRUE(intersects(box5, sph));
    EXPECT_TRUE(intersects(box6, sph));
  }

  TEST(engine, tridi_intersects_box_sphere_2) {
    auto sph = sphere { .center = { 0, 0, 4 }, .radius = 2 };

    auto box1 = box { .min = { -6, -2, 2 }, .max = { -4, 2, 6 } };
    auto box2 = box { .min = { 4, -2, 2 }, .max = { 6, 2, 6 } };
    auto box3 = box { .min = { -2, -6, 2 }, .max = { 2, -4, 6 } };
    auto box4 = box { .min = { -2, 4, 2 }, .max = { 2, 6, 6 } };
    auto box5 = box { .min = { -2, -2, -2 }, .max = { 2, 2, 0 } };
    auto box6 = box { .min = { -2, -2, 8 }, .max = { 2, 2, 10 } };

    EXPECT_FALSE(intersects(box1, sph));
    EXPECT_FALSE(intersects(box2, sph));
    EXPECT_FALSE(intersects(box3, sph));
    EXPECT_FALSE(intersects(box4, sph));
    EXPECT_FALSE(intersects(box5, sph));
    EXPECT_FALSE(intersects(box6, sph));
  }

  TEST(engine, tridi_intersects_box_sphere_3) {
    auto sph = sphere { .center = { 0, 0, 4 }, .radius = 2 };

    auto box1 = box { .min = { -4, -4, 0 }, .max = { -1, -1, 3 } };
    auto box2 = box { .min = { -4, 1, 0 }, .max = { -1, 4, 3 } };
    auto box3 = box { .min = { 1, -4, 0 }, .max = { 4, -1, 3 } };
    auto box4 = box { .min = { 1, 1, 0 }, .max = { 4, 4, 3 } };
    auto box5 = box { .min = { -4, -4, 5 }, .max = { -1, -1, 8 } };
    auto box6 = box { .min = { -4, 1, 5 }, .max = { -1, 4, 8 } };
    auto box7 = box { .min = { 1, -4, 5 }, .max = { 4, -1, 8 } };
    auto box8 = box { .min = { 1, 1, 5 }, .max = { 4, 4, 8 } };

    EXPECT_TRUE(intersects(box1, sph));
    EXPECT_TRUE(intersects(box2, sph));
    EXPECT_TRUE(intersects(box3, sph));
    EXPECT_TRUE(intersects(box4, sph));
    EXPECT_TRUE(intersects(box5, sph));
    EXPECT_TRUE(intersects(box6, sph));
    EXPECT_TRUE(intersects(box7, sph));
    EXPECT_TRUE(intersects(box8, sph));
  }

  TEST(engine, tridi_intersects_box_sphere_4) {
    auto sph = sphere { .center = { 0, 0, 4 }, .radius = 2 };

    auto box1 = box { .min = { -4, -4, 0 }, .max = { -2, -2, 3 } };
    auto box2 = box { .min = { -4, 2, 0 }, .max = { -2, 4, 3 } };
    auto box3 = box { .min = { 2, -4, 0 }, .max = { 4, -2, 3 } };
    auto box4 = box { .min = { 2, 2, 0 }, .max = { 4, 4, 3 } };
    auto box5 = box { .min = { -4, -4, 5 }, .max = { -2, -2, 8 } };
    auto box6 = box { .min = { -4, 2, 5 }, .max = { -2, 4, 8 } };
    auto box7 = box { .min = { 2, -4, 5 }, .max = { 4, -2, 8 } };
    auto box8 = box { .min = { 2, 2, 5 }, .max = { 4, 4, 8 } };

    EXPECT_FALSE(intersects(box1, sph));
    EXPECT_FALSE(intersects(box2, sph));
    EXPECT_FALSE(intersects(box3, sph));
    EXPECT_FALSE(intersects(box4, sph));
    EXPECT_FALSE(intersects(box5, sph));
    EXPECT_FALSE(intersects(box6, sph));
    EXPECT_FALSE(intersects(box7, sph));
    EXPECT_FALSE(intersects(box8, sph));
  }

  TEST(engine, tridi_intersects_ray_box_1) {
    auto foo = box { .min = { -2, -2, -2 }, .max = { 2, 2, 2 } };

    auto bar = ray { .base      = { -10, 0, 10 },
                     .direction = { 10, 0, -10 } };

    auto bus = ray { .base      = { -10, 0, 10 },
                     .direction = { 10, 0, 10 } };

    EXPECT_TRUE(intersects(bar, foo));
    EXPECT_FALSE(intersects(bus, foo));
  }

  TEST(engine, tridi_intersects_ray_box_2) {
    auto foo = ray { .base = { 0, 0, 0 }, .direction = { 10, 2, 3 } };
    auto bar = box { .min = { -2, -2, -2 }, .max = { 2, 2, 2 } };

    EXPECT_TRUE(intersects(foo, bar));
  }

  TEST(engine, tridi_intersects_box_triangle_1) {
    auto foo = box { .min = { -2, -2, -2 }, .max = { 2, 2, 2 } };

    auto tri1 = triangle { vec3i { 0, -3, -3 }, vec3i { 0, 3, -3 },
                           vec3i { 0, 0, 3 } };

    auto tri2 = triangle { vec3i { -3, 0, -3 }, vec3i { 3, 0, -3 },
                           vec3i { 0, 0, 3 } };

    auto tri3 = triangle { vec3i { -3, -3, 0 }, vec3i { 3, -3, 0 },
                           vec3i { 0, 3, 0 } };

    auto tri4 = triangle { vec3i { 0, -3, -3 }, vec3i { 0, 3, -3 },
                           vec3i { 0, 0, 0 } };

    auto tri5 = triangle { vec3i { -3, 0, -3 }, vec3i { 3, 0, -3 },
                           vec3i { 0, 0, 0 } };

    auto tri6 = triangle { vec3i { -3, -3, 0 }, vec3i { 3, -3, 0 },
                           vec3i { 0, 0, 0 } };

    EXPECT_TRUE(intersects(foo, tri1));
    EXPECT_TRUE(intersects(foo, tri2));
    EXPECT_TRUE(intersects(foo, tri3));
    EXPECT_TRUE(intersects(foo, tri4));
    EXPECT_TRUE(intersects(foo, tri5));
    EXPECT_TRUE(intersects(foo, tri6));
  }

  TEST(engine, tridi_intersects_box_triangle_2) {
    auto foo = box { .min = { -2, -2, -2 }, .max = { 2, 2, 2 } };

    auto tri1 = triangle { vec3i { 0, -4, 0 }, vec3i { 0, -4, -5 },
                           vec3i { 0, 0, -5 } };

    auto tri2 = triangle { vec3i { 0, -4, 0 }, vec3i { 0, -4, 5 },
                           vec3i { 0, 0, 5 } };

    auto tri3 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 0, -5 },
                           vec3i { 0, 0, -5 } };

    auto tri4 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 0, 5 },
                           vec3i { 0, 0, 5 } };

    auto tri5 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, -5, 0 },
                           vec3i { 0, -5, 0 } };

    auto tri6 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 5, 0 },
                           vec3i { 0, 5, 0 } };

    auto tri7 = triangle { vec3i { -4, 0, 8 }, vec3i { -4, 5, 9 },
                           vec3i { 0, 5, 10 } };

    EXPECT_FALSE(intersects(foo, tri1));
    EXPECT_FALSE(intersects(foo, tri2));
    EXPECT_FALSE(intersects(foo, tri3));
    EXPECT_FALSE(intersects(foo, tri4));
    EXPECT_FALSE(intersects(foo, tri5));
    EXPECT_FALSE(intersects(foo, tri6));
    EXPECT_FALSE(intersects(foo, tri7));
  }

  TEST(engine, tridi_intersects_box_triangle_3) {
    auto foo = box { .min = { -2, -2, -2 }, .max = { 2, 2, 2 } };

    auto tri1 = triangle { vec3i { 0, -4, 0 }, vec3i { 0, -4, -4 },
                           vec3i { 0, 0, -4 } };

    auto tri2 = triangle { vec3i { 0, -4, 0 }, vec3i { 0, -4, 4 },
                           vec3i { 0, 0, 4 } };

    auto tri3 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 0, -4 },
                           vec3i { 0, 0, -4 } };

    auto tri4 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 0, 4 },
                           vec3i { 0, 0, 4 } };

    auto tri5 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, -4, 0 },
                           vec3i { 0, -4, 0 } };

    auto tri6 = triangle { vec3i { -4, 0, 0 }, vec3i { -4, 4, 0 },
                           vec3i { 0, 4, 0 } };

    EXPECT_TRUE(intersects(foo, tri1));
    EXPECT_TRUE(intersects(foo, tri2));
    EXPECT_TRUE(intersects(foo, tri3));
    EXPECT_TRUE(intersects(foo, tri4));
    EXPECT_TRUE(intersects(foo, tri5));
    EXPECT_TRUE(intersects(foo, tri6));
  }

  TEST(engine, tridi_intersects_cylinder) {
    auto foo = cylinder { .base   = { 0, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto bar = cylinder { .base   = { 0, 0, 3 },
                          .radius = 2,
                          .height = 4 };

    auto bus = cylinder { .base   = { 0, 0, -1 },
                          .radius = 2,
                          .height = 4 };

    auto sus = cylinder { .base   = { -2, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto oof = cylinder { .base   = { 0, -2, -2 },
                          .radius = 2,
                          .height = 4 };

    auto rab = cylinder { .base   = { -2, -2, -2 },
                          .radius = 2,
                          .height = 4 };

    auto sub = cylinder { .base   = { -3, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto sob = cylinder { .base   = { 0, -3, -2 },
                          .radius = 2,
                          .height = 4 };

    auto bos = cylinder { .base   = { 0, 0, -7 },
                          .radius = 2,
                          .height = 4 };

    EXPECT_FALSE(intersects(foo, bar));
    EXPECT_TRUE(intersects(foo, bus));
    EXPECT_TRUE(intersects(bar, bus));
    EXPECT_TRUE(intersects(foo, sus));
    EXPECT_TRUE(intersects(foo, oof));
    EXPECT_FALSE(intersects(foo, rab));
    EXPECT_FALSE(intersects(foo, sub));
    EXPECT_FALSE(intersects(foo, sob));
    EXPECT_FALSE(intersects(foo, bos));
  }

  TEST(engine, tridi_intersects_cylinder_sphere_1) {
    auto foo = cylinder { .base   = { 0, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto sph1 = sphere { .center = { 0, 0, 0 }, .radius = 1 };
    auto sph2 = sphere { .center = { 0, 0, -3 }, .radius = 1 };
    auto sph3 = sphere { .center = { 0, 0, 3 }, .radius = 1 };
    auto sph4 = sphere { .center = { 0, -3, 0 }, .radius = 1 };
    auto sph5 = sphere { .center = { -3, 0, 0 }, .radius = 1 };
    auto sph6 = sphere { .center = { 0, 3, 0 }, .radius = 1 };
    auto sph7 = sphere { .center = { 3, 0, 0 }, .radius = 1 };

    EXPECT_TRUE(intersects(foo, sph1));
    EXPECT_TRUE(intersects(foo, sph2));
    EXPECT_TRUE(intersects(foo, sph3));
    EXPECT_TRUE(intersects(foo, sph4));
    EXPECT_TRUE(intersects(foo, sph5));
    EXPECT_TRUE(intersects(foo, sph6));
    EXPECT_TRUE(intersects(foo, sph7));
  }

  TEST(engine, tridi_intersects_cylinder_sphere_2) {
    auto foo = cylinder { .base   = { 0, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto sph1  = sphere { .center = { -5, -5, -5 }, .radius = 1 };
    auto sph2  = sphere { .center = { 0, 0, -4 }, .radius = 1 };
    auto sph3  = sphere { .center = { 0, 0, 4 }, .radius = 1 };
    auto sph4  = sphere { .center = { 0, -4, 0 }, .radius = 1 };
    auto sph5  = sphere { .center = { -4, 0, 0 }, .radius = 1 };
    auto sph6  = sphere { .center = { 0, 4, 0 }, .radius = 1 };
    auto sph7  = sphere { .center = { 4, 0, 0 }, .radius = 1 };
    auto sph8  = sphere { .center = { -3, -3, 0 }, .radius = 1 };
    auto sph9  = sphere { .center = { -3, 3, 0 }, .radius = 1 };
    auto sph10 = sphere { .center = { 3, -3, 0 }, .radius = 1 };
    auto sph11 = sphere { .center = { 3, 3, 0 }, .radius = 1 };

    EXPECT_FALSE(intersects(foo, sph1));
    EXPECT_FALSE(intersects(foo, sph2));
    EXPECT_FALSE(intersects(foo, sph3));
    EXPECT_FALSE(intersects(foo, sph4));
    EXPECT_FALSE(intersects(foo, sph5));
    EXPECT_FALSE(intersects(foo, sph6));
    EXPECT_FALSE(intersects(foo, sph7));
    EXPECT_FALSE(intersects(foo, sph8));
    EXPECT_FALSE(intersects(foo, sph9));
    EXPECT_FALSE(intersects(foo, sph10));
    EXPECT_FALSE(intersects(foo, sph11));
  }

  TEST(engine, tridi_intersects_cylinder_sphere_3) {
    auto foo = cylinder { .base   = { 0, 0, -2 },
                          .radius = 2,
                          .height = 4 };

    auto sph1 = sphere { .center = { -3, 0, 4 }, .radius = 3 };
    auto sph2 = sphere { .center = { -3, 0, 5 }, .radius = 3 };
    auto sph3 = sphere { .center = { -3, 0, -4 }, .radius = 3 };
    auto sph4 = sphere { .center = { -3, 0, -5 }, .radius = 3 };

    EXPECT_TRUE(intersects(foo, sph1));
    EXPECT_FALSE(intersects(foo, sph2));
    EXPECT_TRUE(intersects(foo, sph3));
    EXPECT_FALSE(intersects(foo, sph4));
  }

  TEST(engine, tridi_intersects_sphere) {
    auto foo = sphere { .center = { 0, 0, 0 }, .radius = 2 };

    auto bar = sphere { .center = { 1, 1, 1 }, .radius = 2 };

    auto sub = sphere { .center = { -2, -2, -3 }, .radius = 2 };

    EXPECT_TRUE(intersects(foo, bar));
    EXPECT_FALSE(intersects(foo, sub));
  }
}
