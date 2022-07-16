#include "../../laplace/execution.h"
#include "io_impl.test.h"
#include <catch2/catch.hpp>
#include <chrono>
#include <thread>

namespace laplace::test {
  using std::make_shared, std::this_thread::sleep_for,
      std::chrono::milliseconds, std::chrono::steady_clock;

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
    REQUIRE(execution {}.thread_count() == default_thread_count());
  }

  TEST_CASE("execution set thread count") {
    REQUIRE(execution {}.set_thread_count(0).thread_count() == 0);
    REQUIRE(execution {}.set_thread_count(1).thread_count() == 1);
    REQUIRE(execution {}.set_thread_count(4).thread_count() == 4);
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

  TEST_CASE("seq execution queue action") {
    bool called = false;

    auto exe = execution {}.set_thread_count(0);

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    REQUIRE(!called);
  }

  TEST_CASE("seq execution queue action and schedule") {
    bool called = false;

    auto exe = execution {}.set_thread_count(0);

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    exe.schedule_and_join(1);
    REQUIRE(called);
  }

  TEST_CASE("seq execution queue action and schedule zero") {
    bool called = false;

    auto exe = execution {}.set_thread_count(0);

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          called = true;
          co_return;
        }));

    exe.schedule_and_join(0);
    REQUIRE(!called);
  }

  TEST_CASE("seq execution queue action and access default self") {
    bool ok = false;

    auto exe = execution {}.set_thread_count(0);

    exe.queue(action {}.setup(
        [&](entity self) -> coro::generator<impact_list> {
          ok = self.index_of(1) == index_undefined;
          co_return;
        }));

    exe.schedule_and_join(1);
    REQUIRE(ok);
  }

  TEST_CASE("seq execution queue action and access custom self") {
    bool ok = false;

    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE("seq execution set value impact") {
    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE("seq execution set value impact twice") {
    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE(
      "seq execution set value impact twice with continuation") {
    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE("seq execution two actions") {
    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE("seq execution invalid impact") {
    auto exe = execution {}.set_thread_count(0);

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { integer_set {
              .id = 1, .index = 0, .value = 42 } };
        }));
    exe.schedule_and_join(1);

    REQUIRE(exe.error());
  }

  TEST_CASE("seq execution sync impacts applied first") {
    auto exe = execution {}.set_thread_count(0);

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

  TEST_CASE("seq execution queue action impact") {
    auto exe = execution {}.set_thread_count(0);

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { queue_action { action {}.setup(
              [](entity) -> coro::generator<impact_list> {
                co_yield impact {
                  integer_allocate_into { .id = 0, .size = 1 }
                } + impact { integer_set {
                        .id = 0, .index = 0, .value = 42 } };
              }) } };
        }));

    exe.schedule_and_join(1);

    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  }

  TEST_CASE("execution action order") {
    auto       exe  = execution {}.set_thread_count(2);
    const auto tick = action::default_tick_duration;

    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { noop {} };
          sleep_for(milliseconds(20));
          co_yield impact { queue_action { action {}.setup(
              { [](entity) -> coro::generator<impact_list> {
                sleep_for(milliseconds(20));
                co_yield impact { queue_action { action {}.setup(
                    { [](entity) -> coro::generator<impact_list> {
                      sleep_for(milliseconds(20));
                      co_yield impact { integer_allocate_into {
                          .id = 0, .size = 1 } };
                    } }) } };
              } }) } };
        }));
    exe.queue(
        action {}.setup([](entity) -> coro::generator<impact_list> {
          co_yield impact { noop {} };
          co_yield impact { queue_action { action {}.setup(
              { [](entity) -> coro::generator<impact_list> {
                co_yield impact { queue_action { action {}.setup(
                    { [](entity) -> coro::generator<impact_list> {
                      co_yield impact { integer_deallocate {
                          .id = 0 } };
                    } }) } };
              } }) } };
        }));
    exe.schedule_and_join(1 + tick);

    REQUIRE(!exe.error());
  }

  TEST_CASE("seq execution no multithreading") {
    auto exe = execution {}.set_thread_count(0);

    auto foo_begin = steady_clock::time_point {};
    auto foo_end   = steady_clock::time_point {};
    auto bar_begin = steady_clock::time_point {};
    auto bar_end   = steady_clock::time_point {};

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          foo_begin = steady_clock::now();
          sleep_for(milliseconds(100));
          foo_end = steady_clock::now();
          co_yield impact { noop {} };
        }));
    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          bar_begin = steady_clock::now();
          sleep_for(milliseconds(100));
          bar_end = steady_clock::now();
          co_yield impact { noop {} };
        }));
    exe.schedule_and_join(1);

    REQUIRE((foo_end < bar_begin || bar_end < foo_begin));
  }

  TEST_CASE("execution two parallel actions") {
    auto exe = execution {}.set_thread_count(4);

    auto foo_begin = steady_clock::time_point {};
    auto foo_end   = steady_clock::time_point {};
    auto bar_begin = steady_clock::time_point {};
    auto bar_end   = steady_clock::time_point {};

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          foo_begin = steady_clock::now();
          sleep_for(milliseconds(2000));
          foo_end = steady_clock::now();
          co_yield impact { noop {} };
        }));
    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          bar_begin = steady_clock::now();
          sleep_for(milliseconds(2000));
          bar_end = steady_clock::now();
          co_yield impact { noop {} };
        }));
    exe.schedule_and_join(1);

    REQUIRE((foo_begin < bar_end && bar_begin < foo_end));
  }

  TEST_CASE("execution join one") {
    auto exe = execution {}.set_thread_count(1);

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          sleep_for(milliseconds(100));
          co_yield impact {
            integer_allocate_into { .id = 0, .size = 1 }
          } + impact { integer_set {
                  .id = 0, .index = 0, .value = 42 } };
        }));

    exe.schedule(1);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
    exe.join();
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  }
  
  TEST_CASE("execution join four") {
    auto exe = execution {}.set_thread_count(4);

    exe.queue(
        action {}.setup([&](entity) -> coro::generator<impact_list> {
          sleep_for(milliseconds(100));
          co_yield impact {
            integer_allocate_into { .id = 0, .size = 1 }
          } + impact { integer_set {
                  .id = 0, .index = 0, .value = 42 } };
        }));

    exe.schedule(1);
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
    exe.join();
    REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  }

  TEST_CASE("execution set value impact") {
    auto exe = execution {}.set_thread_count(4);

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
}
