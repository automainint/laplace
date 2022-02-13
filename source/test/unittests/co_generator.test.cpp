/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/coroutine/generator.h"
#include "../../laplace/coroutine/task.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using coroutine::generator, coroutine::task;

  TEST(coroutine, generator_for) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
    };

    int value = 1;
    for (auto n : range(1, 6)) EXPECT_EQ(n, value++);
  }

  TEST(coroutine, generator_call) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
    };

    auto gen = range(1, 6);

    EXPECT_EQ(gen.get(), 1);
    EXPECT_EQ(gen.get(), 2);
    EXPECT_EQ(gen.get(), 3);
    EXPECT_EQ(gen.get(), 4);
    EXPECT_EQ(gen.get(), 5);
    EXPECT_EQ(gen.get(), 5);
  }

  TEST(coroutine, generator_return) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
      co_return end;
    };

    auto gen = range(1, 6);

    EXPECT_EQ(gen.get(), 1);
    EXPECT_EQ(gen.get(), 2);
    EXPECT_EQ(gen.get(), 3);
    EXPECT_EQ(gen.get(), 4);
    EXPECT_EQ(gen.get(), 5);
    EXPECT_EQ(gen.get(), 6);
    EXPECT_EQ(gen.get(), 6);
  }

  TEST(coroutine, generator_await_1) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
      co_return 0;
    };

    auto foo = [&]() -> task<int> {
      auto gen = range(1, 6);
      auto sum = 0;
      while (!gen.is_done()) sum += co_await gen;
      co_return sum;
    };

    EXPECT_EQ(foo().get(), 15);
  }

  TEST(coroutine, generator_await_2) {
    auto foo = [](int begin) -> task<int> {
      co_return begin * 3 - 1;
    };

    auto range = [&](int begin) -> generator<int> {
      int end = co_await foo(begin);
      for (int i = begin; i < end; i++) co_yield i;
    };

    auto bar = range(2);

    EXPECT_EQ(bar.get(), 2);
    EXPECT_EQ(bar.get(), 3);
    EXPECT_EQ(bar.get(), 4);
  }

  TEST(coroutine, generator_move) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
      co_return 0;
    };
    auto foo = range(1, 6);
    auto bar = std::move(foo);
    EXPECT_EQ(bar.get(), 1);
    EXPECT_EQ(bar.get(), 2);
    EXPECT_EQ(bar.get(), 3);
    EXPECT_EQ(bar.get(), 4);
    EXPECT_EQ(bar.get(), 5);
    EXPECT_EQ(bar.get(), 0);
    bar = range(3, 5);
    EXPECT_EQ(bar.get(), 3);
    EXPECT_EQ(bar.get(), 4);
    EXPECT_EQ(bar.get(), 0);
  }

  TEST(coroutine, generator_copy) {
    auto range = [](int begin, int end) -> generator<int> {
      for (int i = begin; i < end; i++) co_yield i;
      co_return 0;
    };
    auto foo = range(1, 6);
    auto bar = decltype(foo) { foo };
    EXPECT_EQ(foo.get(), 1);
    EXPECT_EQ(bar.get(), 2);
    EXPECT_EQ(foo.get(), 3);
    EXPECT_EQ(bar.get(), 4);
    EXPECT_EQ(foo.get(), 5);
    EXPECT_EQ(bar.get(), 0);
    foo = range(3, 5);
    bar = decltype(foo) { foo };
    EXPECT_EQ(bar.get(), 3);
    EXPECT_EQ(foo.get(), 4);
    EXPECT_EQ(bar.get(), 0);
  }

  TEST(coroutine, generator_exception) {
    auto foo = []() -> generator<int> {
      throw 2;
      co_yield 1;
    };
    int thrown_value = -1;
    try {
      std::ignore = foo().get();
    } catch (int &value) { thrown_value = value; }
    EXPECT_EQ(thrown_value, 2);
  }
}
