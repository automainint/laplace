/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/coroutine/sentinel.h"
#include "../../laplace/coroutine/task.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using coroutine::task, coroutine::sentinel;

  TEST(coroutine, task_return) {
    auto foo = []() -> task<int> { co_return 42; };
    EXPECT_EQ(foo().get(), 42);
  }

  TEST(coroutine, task_void) {
    bool run = false;

    auto foo = [&]() -> task<> {
      run = true;
      co_return;
    };

    auto bar = foo();
    EXPECT_FALSE(run);
    bar.resume();
    EXPECT_TRUE(run);
  }

  TEST(coroutine, task_await) {
    bool run = false;

    auto foo = [&]() -> task<> {
      run = true;
      co_return;
    };

    auto bar = [&]() -> task<> { co_await foo(); };

    auto bus = bar();
    EXPECT_FALSE(run);
    bus.resume();
    EXPECT_TRUE(run);
  }

  TEST(coroutine, task_nested) {
    auto foo = [&]() -> task<int> { co_return 42; };
    auto bar = [&]() -> task<int> { co_return co_await foo(); };

    EXPECT_EQ(bar().get(), 42);
  }

  TEST(coroutine, task_sentinel) {
    bool run = false;
    auto foo = [&]() -> task<> {
      co_await sentinel;
      run = true;
    };
    auto bar = foo();
    bar.resume();
    EXPECT_FALSE(run);
    bar.resume();
    EXPECT_TRUE(run);
  }

  TEST(coroutine, task_move_copy_1) {
    auto foo = []() -> task<int> { co_return 42; };

    auto a = foo();
    auto b = std::move(a);
    auto c = decltype(b) { b };
    EXPECT_EQ(c.get(), 42);
    EXPECT_TRUE(b.is_done());
    a = foo();
    b = std::move(a);
    c = decltype(b) { b };
    EXPECT_EQ(c.get(), 42);
    EXPECT_TRUE(b.is_done());
  }

  TEST(coroutine, task_move_copy_2) {
    auto foo = []() -> task<> { co_return; };

    auto a = foo();
    auto b = std::move(a);
    auto c = decltype(b) { b };
    c.resume();
    EXPECT_TRUE(b.is_done());
    a = foo();
    b = std::move(a);
    c = decltype(b) { b };
    c.resume();
    EXPECT_TRUE(b.is_done());
  }

  TEST(coroutine, task_exception_1) {
    auto foo = []() -> task<int> {
      throw 3;
      co_return 1;
    };
    int thrown_value = -1;
    try {
      std::ignore = foo().get();
    } catch (int &value) { thrown_value = value; }
    EXPECT_EQ(thrown_value, 3);
  }

  TEST(coroutine, task_exception_2) {
    auto foo = []() -> task<> {
      throw 4;
      co_return;
    };
    int thrown_value = -1;
    try {
      foo().resume();
    } catch (int &value) { thrown_value = value; }
    EXPECT_EQ(thrown_value, 4);
  }
}
