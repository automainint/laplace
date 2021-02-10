/*  laplace/unittests/e_world.test.cpp
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

namespace laplace::engine::world_test {
  class my_entity : public basic_entity {
  public:
    my_entity(bool is_dynamic) :
        basic_entity(is_dynamic, false, false, false, 10,
                     box {}) {
      using namespace object;

      setup_sets({ { sets::debug_value, 0, 0 } });

      n_value = index_of(sets::debug_value);
    }

    ~my_entity() override = default;

    void tick(access::world) override {
      apply_delta(n_value, 1);
    }

  private:
    size_t n_value = 0;
  };

  class my_action : public basic_impact {
  public:
    my_action(size_t id_entity, int64_t delta) {
      m_id    = id_entity;
      m_delta = delta;
    }

    ~my_action() override = default;

    void perform(access::world w) const override {
      using namespace object;

      auto e = w.get_entity(m_id);
      e.apply_delta(e.index_of(sets::debug_value), m_delta);
    }

  private:
    size_t  m_id    = 0;
    int64_t m_delta = 0;
  };
}

using namespace laplace;
using namespace engine;
using namespace object;
using namespace world_test;
using namespace std;
using namespace chrono;

TEST(laplace_engine, world_single_thread) {
  auto a = make_shared<world>();
  auto e = make_shared<my_entity>(true);

  a->set_thread_count(0);

  a->spawn(e, id_undefined);
  a->tick(100);

  auto value = e->get(e->index_of(sets::debug_value));

  EXPECT_EQ(value, 10);
}

TEST(laplace_engine, world_multithreading) {
  auto a = make_shared<world>();
  auto e = make_shared<my_entity>(true);

  a->set_thread_count(32);

  a->spawn(e, id_undefined);

  a->tick(100);

  auto value = e->get(e->index_of(sets::debug_value));

  EXPECT_EQ(value, 10);
}

TEST(laplace_engine, world_async_impacts) {
  auto a = make_shared<world>();
  auto e = make_shared<my_entity>(false);

  a->set_thread_count(32);

  const auto id = a->spawn(e, id_undefined);

  for (size_t i = 0; i < 100; i++) {
    a->queue(make_shared<my_action>(id, 1));
    a->queue(make_shared<my_action>(id, -1));
  }

  for (size_t i = 0; i < 100; i++) {
    a->queue(make_shared<my_action>(id, 1));
  }

  a->tick(1);

  auto value = e->get(e->index_of(sets::debug_value));

  EXPECT_EQ(value, 100);
}
