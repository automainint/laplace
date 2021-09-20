/*  test/unittests/e_entity.test.cpp
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

#include "../../laplace/engine/basic_entity.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using std::make_shared, std::make_unique, engine::basic_entity,
      std::thread;

  namespace sets = engine::object::sets;

  class my_entity : public basic_entity {
  public:
    my_entity() {
      setup_sets({ { sets::debug_value, 0, 0 } });
    }

    ~my_entity() override = default;
  };

  TEST(engine, entity_bytes) {
    auto e = basic_entity{};

    e.bytes_resize(10);
    e.bytes_set(2, 1);
    e.bytes_set(4, 2);
    e.bytes_set(6, 3);

    EXPECT_EQ(e.bytes_get(2), 0);
    EXPECT_EQ(e.bytes_get(4), 0);
    EXPECT_EQ(e.bytes_get(6), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(2), 1);
    EXPECT_EQ(e.bytes_get(4), 2);
    EXPECT_EQ(e.bytes_get(6), 3);
  }

  TEST(engine, entity_bytes2) {
    auto e = basic_entity{};

    e.bytes_resize(10);
    e.bytes_set(5, 1);
    e.bytes_set(6, 2);
    e.bytes_set(7, 3);

    EXPECT_EQ(e.bytes_get(5), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(7), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(5), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(7), 3);
  }

  TEST(engine, entity_bytes3) {
    auto e = basic_entity{};

    e.bytes_resize(10);
    e.bytes_set(7, 3);
    e.bytes_set(6, 2);
    e.bytes_set(5, 1);

    EXPECT_EQ(e.bytes_get(5), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(7), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(5), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(7), 3);
  }

  TEST(engine, entity_bytes4) {
    auto e = basic_entity{};

    e.bytes_resize(10);
    e.bytes_set(4, 1);
    e.bytes_set(6, 2);
    e.bytes_set(8, 3);

    EXPECT_EQ(e.bytes_get(4), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(8), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(4), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(8), 3);
  }

  TEST(engine, entity_bytes5) {
    auto e = basic_entity{};

    e.bytes_resize(10);
    e.bytes_set(5, 1);
    e.bytes_set(7, 2);
    e.bytes_set(6, 3);

    EXPECT_EQ(e.bytes_get(5), 0);
    EXPECT_EQ(e.bytes_get(7), 0);
    EXPECT_EQ(e.bytes_get(6), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(5), 1);
    EXPECT_EQ(e.bytes_get(7), 2);
    EXPECT_EQ(e.bytes_get(6), 3);
  }

  TEST(engine, entity_multithreading) {
    auto obj     = make_shared<my_entity>();
    auto n_value = obj->index_of(sets::debug_value);

    auto fn_inc10 = [=]() {
      for (sl::index i = 0; i < 10; i++) obj->apply_delta(n_value, 1);
    };

    auto fn_inc100 = [=]() {
      for (sl::index i = 0; i < 100; i++)
        obj->apply_delta(n_value, 1);
    };

    auto threads_10 = sl::vector<thread>(10);

    for (auto &t : threads_10) t = thread(fn_inc10);
    for (auto &t : threads_10) t.join();

    obj->adjust();

    auto value1 = obj->get(n_value);

    EXPECT_EQ(value1, 100);

    auto threads_4 = sl::vector<thread>(4);

    for (auto &t : threads_4) t = thread(fn_inc100);
    for (auto &t : threads_4) t.join();

    obj->adjust();

    auto value2 = obj->get(n_value);

    EXPECT_EQ(value2, 500);
  }
}
