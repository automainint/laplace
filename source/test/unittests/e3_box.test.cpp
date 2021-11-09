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
#include "../../laplace/engine/tridi/polygon.h"
#include "../../laplace/engine/tridi/quad.h"
#include "../../laplace/engine/tridi/triangle.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::vec3i, engine::tridi::box, engine::tridi::triangle,
      engine::tridi::quad, engine::tridi::polygon;

  TEST(engine, tridi_box_is_empty) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 1, 2, 3 } };
    auto const bar = box { .min = { 2, 1, 3 }, .max = { 1, 2, 3 } };
    auto const sub = box { .min = { 1, 3, 3 }, .max = { 1, 2, 3 } };
    auto const bus = box { .min = { 1, 2, 3 }, .max = { 1, 2, 2 } };

    EXPECT_TRUE(box {}.is_empty());
    EXPECT_FALSE(foo.is_empty());
    EXPECT_TRUE(bar.is_empty());
    EXPECT_TRUE(sub.is_empty());
    EXPECT_TRUE(bus.is_empty());
  }

  TEST(engine, tridi_box_get_vertex) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 4, 5, 6 } };

    vec3i const vertices[] = { { 1, 2, 3 }, { 1, 2, 6 }, { 1, 5, 3 },
                               { 1, 5, 6 }, { 4, 2, 3 }, { 4, 2, 6 },
                               { 4, 5, 3 }, { 4, 5, 6 } };

    EXPECT_EQ(foo.get_vertex(0), vertices[0]);
    EXPECT_EQ(foo.get_vertex(1), vertices[1]);
    EXPECT_EQ(foo.get_vertex(2), vertices[2]);
    EXPECT_EQ(foo.get_vertex(3), vertices[3]);
    EXPECT_EQ(foo.get_vertex(4), vertices[4]);
    EXPECT_EQ(foo.get_vertex(5), vertices[5]);
    EXPECT_EQ(foo.get_vertex(6), vertices[6]);
    EXPECT_EQ(foo.get_vertex(7), vertices[7]);

    EXPECT_EQ(foo.get_vertex(-1), vertices[7]);
    EXPECT_EQ(foo.get_vertex(8), vertices[7]);
  }

  TEST(engine, tridi_box_get_plane) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 4, 5, 6 } };

    vec3i const normals[] = { { 0, 0, -1 }, { 0, 0, 1 },
                              { 0, -1, 0 }, { 0, 1, 0 },
                              { -1, 0, 0 }, { 1, 0, 0 } };

    EXPECT_EQ(foo.get_plane(0).normal, normals[0]);
    EXPECT_EQ(foo.get_plane(0).base.z(), 3);

    EXPECT_EQ(foo.get_plane(1).normal, normals[1]);
    EXPECT_EQ(foo.get_plane(1).base.z(), 6);

    EXPECT_EQ(foo.get_plane(2).normal, normals[2]);
    EXPECT_EQ(foo.get_plane(2).base.y(), 2);

    EXPECT_EQ(foo.get_plane(3).normal, normals[3]);
    EXPECT_EQ(foo.get_plane(3).base.y(), 5);

    EXPECT_EQ(foo.get_plane(4).normal, normals[4]);
    EXPECT_EQ(foo.get_plane(4).base.x(), 1);

    EXPECT_EQ(foo.get_plane(5).normal, normals[5]);
    EXPECT_EQ(foo.get_plane(5).base.x(), 4);

    EXPECT_EQ(foo.get_plane(-1).normal, normals[5]);
    EXPECT_EQ(foo.get_plane(-1).base.x(), 4);
    EXPECT_EQ(foo.get_plane(6).normal, normals[5]);
    EXPECT_EQ(foo.get_plane(6).base.x(), 4);
  }

  TEST(engine, tridi_box_get_flat_center) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 5, 6, 7 } };

    EXPECT_EQ(foo.get_flat_center(0), 3);
    EXPECT_EQ(foo.get_flat_center(1), 4);
    EXPECT_EQ(foo.get_flat_center(2), 5);

    EXPECT_EQ(foo.get_flat_center(-1), 0);
    EXPECT_EQ(foo.get_flat_center(3), 0);
  }

  TEST(engine, tridi_box_get_center) {
    auto const foo = box { .min = { 1, 2, 3 }, .max = { 5, 6, 7 } };
    auto const center = vec3i { 3, 4, 5 };

    EXPECT_EQ(foo.get_center(), center);
  }

  TEST(engine, tridi_box_append) {
    EXPECT_FALSE(box {}.append(vec3i {}).is_empty());
  }

  TEST(engine, tridi_box_append_and_contains) {
    EXPECT_FALSE(box {}.contains(vec3i {}));
    EXPECT_TRUE(box {}.append(vec3i {}).contains(vec3i {}));

    auto const a = vec3i { 1, 0, 0 };
    auto const b = vec3i { 0, 2, 0 };
    auto const c = vec3i { 0, 0, 3 };

    auto const foo = box {}.append(a);
    auto const bar = foo.append(b);
    auto const sub = bar.append(c);

    EXPECT_TRUE(foo.contains(a));
    EXPECT_FALSE(foo.contains(b));
    EXPECT_FALSE(foo.contains(c));

    EXPECT_TRUE(bar.contains(a));
    EXPECT_TRUE(bar.contains(b));
    EXPECT_FALSE(bar.contains(c));

    EXPECT_TRUE(sub.contains(a));
    EXPECT_TRUE(sub.contains(b));
    EXPECT_TRUE(sub.contains(c));
  }

  TEST(engine, tridi_box_append_triangle) {
    auto const a = vec3i { 3, 0, 0 };
    auto const b = vec3i { 0, 2, 0 };
    auto const c = vec3i { 0, 0, 1 };

    auto const tri = triangle { a, b, c };
    auto const foo = box {}.append(tri);

    EXPECT_TRUE(foo.contains(a));
    EXPECT_TRUE(foo.contains(b));
    EXPECT_TRUE(foo.contains(c));
    EXPECT_TRUE(foo.contains(tri));
  }

  TEST(engine, tridi_box_append_quad) {
    auto const a = vec3i { 3, 0, 0 };
    auto const b = vec3i { 0, 2, 0 };
    auto const c = vec3i { 0, 0, 1 };
    auto const d = vec3i { 0, 0, -4 };

    auto const qua = quad { a, b, c, d };
    auto const foo = box {}.append(qua);

    EXPECT_TRUE(foo.contains(a));
    EXPECT_TRUE(foo.contains(b));
    EXPECT_TRUE(foo.contains(c));
    EXPECT_TRUE(foo.contains(d));
  }

  TEST(engine, tridi_box_append_polygon) {
    auto const a = vec3i { 3, 0, 0 };
    auto const b = vec3i { 0, 2, 0 };
    auto const c = vec3i { 0, 0, 1 };
    auto const d = vec3i { 0, 0, -4 };
    auto const e = vec3i { -5, 0, 0 };

    auto const pol = polygon { a, b, c, d, e };
    auto const foo = box {}.append(pol);

    EXPECT_TRUE(foo.contains(a));
    EXPECT_TRUE(foo.contains(b));
    EXPECT_TRUE(foo.contains(c));
    EXPECT_TRUE(foo.contains(d));
    EXPECT_TRUE(foo.contains(e));
  }

  TEST(engine, tridi_box_contains_triangle) {
    EXPECT_FALSE(box {}.contains(triangle {}));

    auto const a = vec3i { 3, 0, 0 };
    auto const b = vec3i { 0, 2, 0 };
    auto const c = vec3i { 0, 0, 1 };

    auto const tri = triangle { a, b, c };

    EXPECT_FALSE(box {}.append(a).contains(tri));
    EXPECT_FALSE(box {}.append(a).append(b).contains(tri));
  }

  TEST(engine, tridi_box_contains_box) {
    auto const foo = box { .min = { -1, -2, -3 },
                           .max = { 1, 2, 3 } };

    auto const bar = box { .min = { -1, -2, -3 },
                           .max = { 0, -1, -2 } };

    auto const sub = box { .min = { 0, 1, 2 }, .max = { 1, 2, 3 } };

    auto const bus = box { .min = { -2, -2, -3 },
                           .max = { 0, -1, -2 } };

    EXPECT_TRUE(foo.contains(foo));
    EXPECT_TRUE(foo.contains(bar));
    EXPECT_TRUE(foo.contains(sub));
    EXPECT_FALSE(foo.contains(bus));
  }
}
