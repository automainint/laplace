#include "../../laplace/execution.h"
#include "io_impl.test.h"
#include <chrono>
#include <thread>

#define KIT_TEST_FILE execution
#include <kit_test/test.h>

using std::make_shared, std::this_thread::sleep_for,
    std::chrono::milliseconds;

TEST("create execution") {
  REQUIRE(!laplace::execution {}.error());
}

TEST("execution copy") {
  auto foo = laplace::execution {};
  auto bar = laplace::execution {};
  foo      = bar;
  REQUIRE(!foo.error());
  REQUIRE(!bar.error());
}

TEST("execution move") {
  auto foo = laplace::execution {};
  auto bar = laplace::execution {};
  foo      = std::move(bar);
  REQUIRE(!foo.error());
  REQUIRE(bar.error());
}

TEST("execution get thread count") {
  REQUIRE(laplace::execution {}.thread_count() ==
          laplace::default_thread_count());
}

TEST("execution set thread count") {
  REQUIRE(laplace::execution {}.set_thread_count(0).thread_count() ==
          0);
  REQUIRE(laplace::execution {}.set_thread_count(1).thread_count() ==
          1);
  REQUIRE(laplace::execution {}.set_thread_count(4).thread_count() ==
          4);
}

TEST("execution set thread count to zero") {
  REQUIRE(!laplace::execution {}.set_thread_count(0).error());
}

TEST("execution may fail to set thread count") {
  auto exe = laplace::execution {};
  REQUIRE(exe.set_thread_count(-1).error());
  REQUIRE(exe.set_thread_count(10000000000).error());
  REQUIRE(exe.set_thread_count(10000).error());
}

TEST("execution propagate error") {
  REQUIRE(laplace::execution {}
              .set_thread_count(-1)
              .set_thread_count(0)
              .error());
}

TEST("seq execution queue action") {
  bool called = false;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        called = true;
        co_return;
      }));

  REQUIRE(!called);
}

TEST("seq execution queue action and schedule") {
  bool called = false;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        called = true;
        co_return;
      }));

  exe.schedule_and_join(1);
  REQUIRE(called);
}

TEST("seq execution queue action and schedule zero") {
  bool called = false;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        called = true;
        co_return;
      }));

  exe.schedule_and_join(0);
  REQUIRE(!called);
}

TEST("seq execution queue action and access default self") {
  bool ok = false;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity self)
          -> coro::generator<laplace::impact_list> {
        ok = self.index_of(1) == laplace::index_undefined;
        co_return;
      }));

  exe.schedule_and_join(1);
  REQUIRE(ok);
}

TEST("seq execution queue action and access custom self") {
  bool ok = false;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}
                .set_self(laplace::entity {}.setup({ { .id = 1 } }))
                .setup([&](laplace::entity self)
                           -> coro::generator<laplace::impact_list> {
                  ok = self.index_of(1) == 0;
                  co_return;
                }));

  exe.schedule_and_join(1);
  REQUIRE(ok);
}

TEST("execution read-only") {
  REQUIRE(laplace::execution {}.read_only().get_integer(0, 0, -1) ==
          -1);
}

TEST("execution set state") {
  REQUIRE(laplace::execution {}
              .set_state(laplace::state {
                  make_shared<laplace::test::test_io_impl>() })
              .read_only()
              .get_integer(0, 0, -1) == 42);

  REQUIRE(laplace::execution {}
              .set_state(laplace::state {
                  make_shared<laplace::test::test_io_impl>() })
              .read_only()
              .get_byte(0, 0, -1) == 24);
}

TEST("execution state copy") {
  auto s      = laplace::state {};
  std::ignore = s.apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } });

  auto foo = laplace::execution {};
  auto bar = laplace::execution {}.set_state(s);
  foo      = bar;
  REQUIRE(foo.read_only().get_integer(0, 0, -1) == 0);
  REQUIRE(bar.read_only().get_integer(0, 0, -1) == 0);
}

TEST("execution state move") {
  auto s      = laplace::state {};
  std::ignore = s.apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } });

  auto foo = laplace::execution {};
  auto bar = laplace::execution {}.set_state(s);
  foo      = std::move(bar);
  REQUIRE(foo.read_only().get_integer(0, 0, -1) == 0);
  REQUIRE(bar.read_only().get_integer(0, 0, -1) == -1);
}

TEST("seq execution set value impact") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("seq execution set value impact twice") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("seq execution set value impact twice with continuation") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } } +
            laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } } +
            laplace::impact { laplace::tick_continue {} };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("seq execution two actions") {
  auto exe = laplace::execution {}.set_thread_count(0);

  const auto tick = laplace::action::default_tick_duration;

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 18 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 24 } };
      }));
  exe.schedule_and_join(tick);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("seq execution invalid impact") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 42 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.error());
}

TEST("seq execution sync impacts applied first") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_set { .id = 0, .index = 0, .value = 42 }
        } + laplace::impact { laplace::integer_allocate_into {
                .id = 0, .size = 1 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 43 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 1, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  REQUIRE(exe.read_only().get_integer(1, 0, -1) == 43);
}

TEST("seq execution queue action impact") {
  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::queue_action {
            laplace::action {}.setup(
                [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                  co_yield laplace::impact {
                    laplace::integer_allocate_into { .id   = 0,
                                                     .size = 1 }
                  } + laplace::impact { laplace::integer_set {
                          .id = 0, .index = 0, .value = 42 } };
                }) } };
      }));

  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("seq execution no multithreading") {
  static auto m         = std::mutex {};
  static auto foo_begin = 0;
  static auto foo_end   = 0;
  static auto bar_begin = 0;
  static auto bar_end   = 0;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        foo_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        foo_end = bar_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        bar_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        bar_end = foo_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.schedule_and_join(1);

  REQUIRE(foo_end + bar_end == 1);
}

TEST("execution two parallel actions") {
  static auto m         = std::mutex {};
  static auto foo_begin = 0;
  static auto foo_end   = 0;
  static auto bar_begin = 0;
  static auto bar_end   = 0;

  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        foo_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        foo_end = bar_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        bar_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        bar_end = foo_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.schedule_and_join(1);

  REQUIRE(foo_end == 1);
  REQUIRE(bar_end == 1);
}

TEST("execution join one") {
  auto exe = laplace::execution {}.set_thread_count(1);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        sleep_for(milliseconds(100));
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
      }));

  exe.schedule(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
  exe.join();
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("execution join four") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        sleep_for(milliseconds(100));
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
      }));

  exe.schedule(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
  exe.join();
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("execution set value impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("execution set value impact twice") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("execution set value impact twice with continuation") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } } +
            laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } } +
            laplace::impact { laplace::tick_continue {} };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("execution two actions") {
  auto exe = laplace::execution {}.set_thread_count(4);

  const auto tick = laplace::action::default_tick_duration;

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 18 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 24 } };
      }));
  exe.schedule_and_join(tick);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("execution invalid impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 42 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.error());
}

TEST("execution sync impacts applied first") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_set { .id = 0, .index = 0, .value = 42 }
        } + laplace::impact { laplace::integer_allocate_into {
                .id = 0, .size = 1 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 43 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 1, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  REQUIRE(exe.read_only().get_integer(1, 0, -1) == 43);
}

TEST("execution queue action impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::queue_action {
            laplace::action {}.setup(
                [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                  co_yield laplace::impact {
                    laplace::integer_allocate_into { .id   = 0,
                                                     .size = 1 }
                  } + laplace::impact { laplace::integer_set {
                          .id = 0, .index = 0, .value = 42 } };
                }) } };
      }));

  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("execution action order") {
  auto       exe  = laplace::execution {}.set_thread_count(4);
  const auto tick = laplace::action::default_tick_duration;

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::noop {} };
        sleep_for(milliseconds(20));
        co_yield laplace::impact {
          laplace::queue_action { laplace::action {}.setup(
              { [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                sleep_for(milliseconds(20));
                co_yield laplace::impact {
                  laplace::queue_action { laplace::action {}.setup(
                      { [](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                        sleep_for(milliseconds(20));
                        co_yield laplace::impact {
                          laplace::integer_allocate_into { .id   = 0,
                                                           .size = 1 }
                        };
                      } }) }
                };
              } }) }
        };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::noop {} };
        co_yield laplace::impact {
          laplace::queue_action { laplace::action {}.setup(
              { [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                co_yield laplace::impact {
                  laplace::queue_action { laplace::action {}.setup(
                      { [](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                        co_yield laplace::impact {
                          laplace::integer_deallocate { .id = 0 }
                        };
                      } }) }
                };
              } }) }
        };
      }));
  exe.schedule_and_join(1 + tick);

  REQUIRE(!exe.error());
}
