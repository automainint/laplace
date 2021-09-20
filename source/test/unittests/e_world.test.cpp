/*  test/unittests/e_world.test.cpp
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

#include "../../laplace/engine/access/world.h"
#include "../../laplace/engine/basic_impact.h"
#include "../../laplace/engine/world.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using std::make_shared, std::make_unique, std::thread, engine::basic_entity,
      engine::basic_impact, engine::world, engine::scheduler,
      engine::id_undefined;

  namespace access = engine::access;
  namespace sets   = engine::object::sets;

  class my_counter : public basic_entity {
  public:
    my_counter() : basic_entity(proto) {
      setup_sets({ { sets::debug_value, 0, 0 } });
      n_value = index_of(sets::debug_value);
    }

    my_counter(dynamic_tag) : basic_entity(dynamic) {
      setup_sets({ { sets::debug_value, 0, 0 } });
      n_value = index_of(sets::debug_value);
    }

    ~my_counter() override = default;

    void tick(access::world) override {
      apply_delta(n_value, 1);
    }

  private:
    sl::index n_value = 0;
  };

  class my_additioner : public basic_impact {
  public:
    my_additioner(sl::index id_entity, int64_t delta) {
      m_id    = id_entity;
      m_delta = delta;
    }

    ~my_additioner() override = default;

    void perform(access::world w) const override {
      auto e = w.get_entity(m_id);
      e.apply_delta(e.index_of(sets::debug_value), m_delta);
    }

  private:
    sl::index m_id    = 0;
    int64_t   m_delta = 0;
  };

  TEST(engine, world_single_thread) {
    auto a = make_shared<world>();
    auto e = make_shared<my_counter>(my_counter::dynamic);

    a->set_thread_count(0);

    a->spawn(e, id_undefined);
    a->tick(100);

    auto value = e->get(e->index_of(sets::debug_value));

    EXPECT_EQ(value, 10);
  }

  TEST(engine, world_multithreading) {
    auto a = make_shared<world>();
    auto e = make_shared<my_counter>(my_counter::dynamic);

    const auto max_threads = scheduler::overthreading_limit *
                             thread::hardware_concurrency();

    a->set_thread_count(max_threads);

    a->spawn(e, id_undefined);
    a->tick(100);

    auto value = e->get(e->index_of(sets::debug_value));

    EXPECT_EQ(value, 10);
  }

  TEST(engine, world_async_impacts) {
    auto a = make_shared<world>();
    auto e = make_shared<my_counter>();

    const auto max_threads = scheduler::overthreading_limit *
                             thread::hardware_concurrency();

    a->set_thread_count(max_threads);

    const auto id = a->spawn(e, id_undefined);

    for (sl::index i = 0; i < 100; i++) {
      a->queue(make_unique<my_additioner>(id, 1));
      a->queue(make_unique<my_additioner>(id, -1));
    }

    for (sl::index i = 0; i < 100; i++) {
      a->queue(make_unique<my_additioner>(id, 1));
    }

    a->tick(1);

    auto value = e->get(e->index_of(sets::debug_value));

    EXPECT_EQ(value, 100);
  }
}
