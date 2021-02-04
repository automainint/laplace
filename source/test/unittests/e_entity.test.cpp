/*  laplace/unittests/e_entity.test.cpp
 *
 *      LLVM libFuzzer entry.
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

namespace laplace::engine::entity_test {
  class my_entity : public basic_entity {
  public:
    my_entity();
    ~my_entity() override;
  };

  my_entity::my_entity() {
    setup_sets({ { object::sets::debug_value, 0, 0 } });
  }

  my_entity::~my_entity() { }
}

using namespace laplace;
using namespace engine;
using namespace object;
using namespace entity_test;
using namespace std;

TEST(laplace_engine, entity_multithreading) {
  shared_ptr<my_entity> obj = make_shared<my_entity>();
  auto n_value              = obj->index_of(sets::debug_value);

  auto fn_inc10 = [=]() {
    for (size_t i = 0; i < 10; i++)
      obj->apply_delta(n_value, 1);
  };

  auto fn_inc100 = [=]() {
    for (size_t i = 0; i < 100; i++)
      obj->apply_delta(n_value, 1);
  };

  vector<unique_ptr<thread>> threads_10(10);

  for (size_t i = 0; i < threads_10.size(); i++)
    threads_10[i] = make_unique<thread>(fn_inc10);

  for (size_t i = 0; i < threads_10.size(); i++)
    threads_10[i]->join();

  obj->adjust();

  auto value1 = obj->get(n_value);

  EXPECT_EQ(value1, 100);

  vector<unique_ptr<thread>> threads_4(4);

  for (size_t i = 0; i < threads_4.size(); i++)
    threads_4[i] = make_unique<thread>(fn_inc100);

  for (size_t i = 0; i < threads_4.size(); i++)
    threads_4[i]->join();

  obj->adjust();

  auto value2 = obj->get(n_value);

  EXPECT_EQ(value2, 500);
}
