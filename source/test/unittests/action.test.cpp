#include "../../laplace/action.h"
#include "../../laplace/impact.h"

#define KIT_TEST_FILE action
#include <kit_test/test.h>

namespace laplace::test {
  TEST("create action") {
    REQUIRE(!action {}.error());
  }

  TEST("action default tick duration") {
    REQUIRE(action {}.tick_duration() ==
            action::default_tick_duration);
  }

  TEST("action set tick duration") {
    REQUIRE(action {}.set_tick_duration(42).tick_duration() == 42);
  }

  TEST("Set tick duration may fail") {
    REQUIRE(action {}.set_tick_duration(0).error());
    REQUIRE(action {}.set_tick_duration(-1).error());
  }

  TEST("action run default") {
    auto run = action {}.run({});
    run.resume();
    REQUIRE(run.is_done());
  }

  TEST("action setup and run") {
    bool called = false;

    auto run = action {}
                   .setup(
                       [&](entity) -> coro::generator<impact_list> {
                         called = true;
                         co_return;
                       })
                   .run({});
    run.resume();

    REQUIRE(called);
  }

  TEST("action propagate error") {
    REQUIRE(
        action {}.set_tick_duration(-1).set_tick_duration(1).error());
  }

  TEST("action propagate error and run") {
    bool called = false;

    auto run = action {}
                   .set_tick_duration(-1)
                   .setup(
                       [&](entity) -> coro::generator<impact_list> {
                         called = true;
                         co_return;
                       })
                   .run({});
    run.resume();

    REQUIRE(!called);
  }

  TEST("action setup first, propagate error and run") {
    bool called = false;

    auto run = action {}
                   .setup(
                       [&](entity) -> coro::generator<impact_list> {
                         called = true;
                         co_return;
                       })
                   .set_tick_duration(-1)
                   .run({});
    run.resume();

    REQUIRE(!called);
  }
}
