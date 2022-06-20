/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/execution.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("Create execution") {
    REQUIRE(!execution {}.is_error());
  }

  TEST_CASE("Execution copy") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = bar;
    REQUIRE(!foo.is_error());
  }

  TEST_CASE("Execution move") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = std::move(bar);
    REQUIRE(!foo.is_error());
    REQUIRE(bar.is_error());
  }

  TEST_CASE("Execution get thread count") {
    REQUIRE(execution {}.get_thread_count() ==
            execution::default_thread_count);
  }

  TEST_CASE("Execution set thread count") {
    auto exe = execution {}.set_thread_count(4);
    REQUIRE(!exe.is_error());
    REQUIRE(exe.get_thread_count() == 4);
  }

  TEST_CASE("Execution set thread count to zero") {
    REQUIRE(!execution {}.set_thread_count(0).is_error());
  }

  TEST_CASE("Execution may fail to set thread count") {
    auto exe = execution {};
    REQUIRE(exe.set_thread_count(-1).is_error());
    REQUIRE(exe.set_thread_count(10000000000).is_error());
    REQUIRE(exe.set_thread_count(10000).is_error());
  }

  TEST_CASE("Execution propagate error") {
    REQUIRE(execution {}
                .set_thread_count(-1)
                .set_thread_count(0)
                .is_error());
  }

  TEST_CASE("Execution queue action") {
    bool called = false;

    auto exe = execution {};

    exe.queue(action {}.setup([&](entity) -> coro::generator<impact> {
      called = true;
      co_return;
    }));

    REQUIRE(!called);
  }

  TEST_CASE("Execution queue action and schedule") {
    bool called = false;

    auto exe = execution {};

    exe.queue(action {}.setup([&](entity) -> coro::generator<impact> {
      called = true;
      co_return;
    }));

    exe.schedule(1);
    exe.join();

    REQUIRE(called);
  }

  TEST_CASE("Execution queue action and access default self") {
    bool ok = false;

    auto exe = execution {};

    exe.queue(
        action {}.setup([&](entity self) -> coro::generator<impact> {
          ok = self.index_of(1) == index_undefined;
          co_return;
        }));

    exe.schedule(1);
    exe.join();

    REQUIRE(ok);
  }

  TEST_CASE("Execution queue action and access custom self") {
    bool ok = false;

    auto exe = execution {};

    exe.queue(action {}
                  .set_self(entity {}.setup({ { .id = 1 } }))
                  .setup([&](entity self) -> coro::generator<impact> {
                    ok = self.index_of(1) == 0;
                    co_return;
                  }));

    exe.schedule(1);
    exe.join();

    REQUIRE(ok);
  }
}
