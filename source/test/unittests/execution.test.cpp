#include "../../laplace/execution.h"
#include "io_impl.test.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  using std::make_shared;

  TEST_CASE("create execution") {
    REQUIRE(!execution {}.error());
  }

  TEST_CASE("execution copy") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = bar;
    REQUIRE(!foo.error());
    REQUIRE(!bar.error());
  }

  TEST_CASE("execution move") {
    auto foo = execution {};
    auto bar = execution {};
    foo      = std::move(bar);
    REQUIRE(!foo.error());
    REQUIRE(bar.error());
  }

  TEST_CASE("execution get thread count") {
    REQUIRE(execution {}.thread_count() == default_thread_count);
  }

  TEST_CASE("execution set thread count") {
    auto exe = execution {}.set_thread_count(4);
    REQUIRE(!exe.error());
    REQUIRE(exe.thread_count() == 4);
  }

  TEST_CASE("execution set thread count to zero") {
    REQUIRE(!execution {}.set_thread_count(0).error());
  }

  TEST_CASE("execution may fail to set thread count") {
    auto exe = execution {};
    REQUIRE(exe.set_thread_count(-1).error());
    REQUIRE(exe.set_thread_count(10000000000).error());
    REQUIRE(exe.set_thread_count(10000).error());
  }

  TEST_CASE("execution propagate error") {
    REQUIRE(execution {}
                .set_thread_count(-1)
                .set_thread_count(0)
                .error());
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

    exe.schedule_and_join(1);
    REQUIRE(called);
  }

  TEST_CASE("execution queue action and schedule zero") {
    bool called = false;

    auto exe = execution {};

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    exe.schedule_and_join(0);
    REQUIRE(!called);
  }

  TEST_CASE("execution queue action and access default self") {
    bool ok = false;

    auto exe = execution {};

    exe.queue(action {}.setup(
        [&](entity self) -> coro::generator<impact_list> {
          ok = self.index_of(1) == index_undefined;
          co_return;
        }));

    exe.schedule_and_join(1);
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

    exe.schedule_and_join(1);
    REQUIRE(ok);
  }

  TEST_CASE("execution read-only") {
    REQUIRE(execution {}.read_only().get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("execution set state") {
    REQUIRE(execution {}
                .set_state(state { make_shared<test_io_impl>() })
                .read_only()
                .get_integer(0, 0, -1) == 42);

    REQUIRE(execution {}
                .set_state(state { make_shared<test_io_impl>() })
                .read_only()
                .get_byte(0, 0, -1) == 24);
  }

  TEST_CASE("execution state copy") {
    auto s      = state {};
    std::ignore = s.apply(impact { integer_allocate_into { 0, 1 } });

    auto foo = execution {};
    auto bar = execution {}.set_state(s);
    foo      = bar;
    REQUIRE(foo.read_only().get_integer(0, 0, -1) == 0);
    REQUIRE(bar.read_only().get_integer(0, 0, -1) == 0);
  }

  TEST_CASE("execution state move") {
    auto s      = state {};
    std::ignore = s.apply(impact { integer_allocate_into { 0, 1 } });

    auto foo = execution {};
    auto bar = execution {}.set_state(s);
    foo      = std::move(bar);
    REQUIRE(foo.read_only().get_integer(0, 0, -1) == 0);
    REQUIRE(bar.read_only().get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("execution set value") {
    auto exe = execution {};

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact {
            integer_allocate_into { .id = 0, .size = 1 }
          } + impact { integer_set {
                  .id = 0, .index = 0, .value = 42 } };
          co_yield impact { integer_set {
              .id = 0, .index = 0, .value = 43 } };
        }));

    exe.schedule_and_join(1);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  }

  TEST_CASE("execution set value twice") {
    auto exe = execution {};

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact {
            integer_allocate_into { .id = 0, .size = 1 }
          } + impact { integer_set {
                  .id = 0, .index = 0, .value = 42 } };
          co_yield impact { integer_set {
              .id = 0, .index = 0, .value = 43 } };
        }));

    const auto tick = action::default_tick_duration;

    exe.schedule_and_join(1);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);

    exe.schedule_and_join(tick);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
  }

  TEST_CASE("execution set value twice with continuation") {
    auto exe = execution {};

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_allocate_into { .id   = 0,
                                                    .size = 1 } } +
              impact { integer_set {
                  .id = 0, .index = 0, .value = 42 } } +
              impact { tick_continue {} };
          co_yield impact { integer_set {
              .id = 0, .index = 0, .value = 43 } };
        }));

    const auto tick = action::default_tick_duration;

    exe.schedule_and_join(1);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);

    exe.schedule_and_join(tick);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
  }

  TEST_CASE("execution two actions") {
    auto exe = execution {};

    const auto tick = action::default_tick_duration;

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_allocate_into { .id   = 0,
                                                    .size = 1 } };
        }));
    exe.schedule_and_join(1);

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_add {
              .id = 0, .index = 0, .delta = 18 } };
        }));
    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_add {
              .id = 0, .index = 0, .delta = 24 } };
        }));
    exe.schedule_and_join(tick);

    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  }

  TEST_CASE("execution invalid impact") {
    auto exe = execution {};

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_set {
              .id = 1, .index = 0, .value = 42 } };
        }));
    exe.schedule_and_join(1);

    REQUIRE(exe.error());
  }

  TEST_CASE("execution sync impacts applied first") {
    auto exe = execution {};

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact {
            integer_set { .id = 0, .index = 0, .value = 42 }
          } + impact { integer_allocate_into { .id = 0, .size = 1 } };
        }));
    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_set {
              .id = 1, .index = 0, .value = 43 } };
        }));
    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_allocate_into { .id   = 1,
                                                    .size = 1 } };
        }));
    exe.schedule_and_join(1);

    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
    REQUIRE(exe.read_only().get_integer(1, 0, -1) == 43);
  }
}
