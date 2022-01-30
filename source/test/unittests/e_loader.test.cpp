/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/loader.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::loader, engine::ptr_prime_impact,
      engine::prime_impact, std::this_thread::sleep_for,
      std::chrono::milliseconds, std::make_unique, std::mutex,
      std::unique_lock, std::string_view;

  TEST(engine, loader_blank) {
    auto l = loader {};
    EXPECT_TRUE(l.is_ready());
    EXPECT_EQ(l.get_progress(), 0);
  }

  TEST(engine, loader_invalid_context_1) {
    setup_log([](string_view) {});
    auto l = loader {};
    l.add_task({});
    sleep_for(milliseconds { 10 });
    EXPECT_TRUE(l.is_ready());
    setup_log({});
  }

  TEST(engine, loader_invalid_context_2) {
    setup_log([](string_view) {});
    auto l = loader {};
    l.on_decode([](span_cbyte) -> ptr_prime_impact { return {}; });
    l.add_task({});
    sleep_for(milliseconds { 10 });
    EXPECT_TRUE(l.is_ready());
    setup_log({});
  }

  TEST(engine, loader_invalid_context_3) {
    setup_log([](string_view) {});
    auto l = loader {};
    l.on_perform([](ptr_prime_impact const &) {});
    l.add_task({});
    sleep_for(milliseconds { 10 });
    EXPECT_TRUE(l.is_ready());
    setup_log({});
  }

  TEST(engine, loader_invalid_task) {
    auto _mtx = mutex {};

    auto l              = loader {};
    bool decode_called  = false;
    bool perform_called = false;

    l.on_decode([&](span_cbyte) -> ptr_prime_impact {
      auto _ul      = unique_lock(_mtx);
      decode_called = true;
      return {};
    });

    l.on_perform([&](ptr_prime_impact const &) {
      auto _ul       = unique_lock(_mtx);
      perform_called = true;
    });

    l.add_task({});
    sleep_for(milliseconds { 10 });

    auto _ul = unique_lock(_mtx);
    EXPECT_TRUE(l.is_ready());
    EXPECT_TRUE(decode_called);
    EXPECT_FALSE(perform_called);
  }

  TEST(engine, loader_single_task) {
    auto _mtx = mutex {};

    auto              l              = loader {};
    bool              decode_called  = false;
    bool              perform_called = false;
    bool              task_match     = false;
    ptr_prime_impact *task           = nullptr;

    l.on_decode([&](span_cbyte) -> ptr_prime_impact {
      auto _ul      = unique_lock(_mtx);
      decode_called = true;
      auto p        = make_unique<prime_impact>();
      task          = &p;
      return p;
    });

    l.on_perform([&](ptr_prime_impact const &p) {
      auto _ul       = unique_lock(_mtx);
      task_match     = &p == task;
      perform_called = true;
    });

    l.add_task({});
    sleep_for(milliseconds { 10 });

    auto _ul = unique_lock(_mtx);
    EXPECT_TRUE(l.is_ready());
    EXPECT_TRUE(decode_called);
    EXPECT_TRUE(perform_called);
    EXPECT_TRUE(task_match);
  }

  TEST(engine, loader_two_tasks) {
    auto _mtx = mutex {};

    auto              l             = loader {};
    int               task1_called  = {};
    int               task2_called  = {};
    bool              correct_order = false;
    ptr_prime_impact *task1         = nullptr;
    ptr_prime_impact *task2         = nullptr;

    l.on_decode([&](span_cbyte seq) -> ptr_prime_impact {
      auto _ul = unique_lock(_mtx);

      auto p = make_unique<prime_impact>();
      if (task1 == &p)
        task1 = nullptr;
      if (task2 == &p)
        task2 = nullptr;

      if (!seq.empty()) {
        if (seq[0] == 1)
          task1 = &p;
        else if (seq[0] == 2)
          task2 = &p;
      }

      return p;
    });

    l.on_perform([&](ptr_prime_impact const &p) {
      auto _ul = unique_lock(_mtx);

      if (&p == task1)
        task1_called++;
      else if (&p == task2)
        task2_called++;

      if (&p == task2 && task1_called == 1)
        correct_order = true;
    });

    l.add_task(vbyte { 1 });
    l.add_task(vbyte { 2 });

    sleep_for(milliseconds { 10 });

    auto _ul = unique_lock(_mtx);
    EXPECT_TRUE(l.is_ready());
    EXPECT_TRUE(correct_order);
    EXPECT_EQ(task1_called, 1);
    EXPECT_EQ(task2_called, 1);
  }
}
