/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/execution.h"
#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  using std::make_shared, std::shared_ptr;

  TEST_CASE("create execution") {
    REQUIRE(!execution {}.is_error());
  }

  TEST_CASE("execution copy") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = bar;
    REQUIRE(!foo.is_error());
    REQUIRE(!bar.is_error());
  }

  TEST_CASE("execution move") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = std::move(bar);
    REQUIRE(!foo.is_error());
    REQUIRE(bar.is_error());
  }

  TEST_CASE("execution get thread count") {
    REQUIRE(execution {}.get_thread_count() ==
            execution::default_thread_count);
  }

  TEST_CASE("execution set thread count") {
    auto exe = execution {}.set_thread_count(4);
    REQUIRE(!exe.is_error());
    REQUIRE(exe.get_thread_count() == 4);
  }

  TEST_CASE("execution set thread count to zero") {
    REQUIRE(!execution {}.set_thread_count(0).is_error());
  }

  TEST_CASE("execution may fail to set thread count") {
    auto exe = execution {};
    REQUIRE(exe.set_thread_count(-1).is_error());
    REQUIRE(exe.set_thread_count(10000000000).is_error());
    REQUIRE(exe.set_thread_count(10000).is_error());
  }

  TEST_CASE("execution propagate error") {
    REQUIRE(execution {}
                .set_thread_count(-1)
                .set_thread_count(0)
                .is_error());
  }

  TEST_CASE("execution queue action") {
    bool called = false;

    auto exe = execution {};

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    REQUIRE(!called);
  }

  TEST_CASE("execution queue action and schedule") {
    bool called = false;

    auto exe = execution {};

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    exe.schedule(1);
    exe.join();

    REQUIRE(called);
  }

  TEST_CASE("execution queue action and access default self") {
    bool ok = false;

    auto exe = execution {};

    exe.queue(action {}.setup(
        [&](entity self) -> coro::generator<impact_list> {
          ok = self.index_of(1) == index_undefined;
          co_return;
        }));

    exe.schedule(1);
    exe.join();

    REQUIRE(ok);
  }

  TEST_CASE("execution queue action and access custom self") {
    bool ok = false;

    auto exe = execution {};

    exe.queue(
        action {}
            .set_self(entity {}.setup({ { .id = 1 } }))
            .setup([&](entity self) -> coro::generator<impact_list> {
              ok = self.index_of(1) == 0;
              co_return;
            }));

    exe.schedule(1);
    exe.join();

    REQUIRE(ok);
  }

  TEST_CASE("execution read-only") {
    REQUIRE(execution {}.read_only().get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("execution set state") {
    struct custom_io_impl : io_interface {
      [[nodiscard]] auto clone() const noexcept
          -> shared_ptr<io_interface> override {
        return make_shared<custom_io_impl>(*this);
      }
      [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                     int_type def) const noexcept
          -> int_type override {
        return 42;
      }
      [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                  byte_type def) const noexcept
          -> byte_type override {
        return 24;
      }
      [[nodiscard]] auto apply(impact const &i) noexcept
          -> bool override {
        return true;
      }
      [[nodiscard]] auto adjust() noexcept -> bool override {
        return false;
      }
      void adjust_done() noexcept override { }
    };

    REQUIRE(execution {}
                .set_state(state { make_shared<custom_io_impl>() })
                .read_only()
                .get_integer(0, 0, -1) == 42);

    REQUIRE(execution {}
                .set_state(state { make_shared<custom_io_impl>() })
                .read_only()
                .get_byte(0, 0, -1) == 24);
  }
}
