#include "../../laplace/x_action.h"
#include "../../laplace/x_impact.h"

#define KIT_TEST_FILE action
#include <kit_test/test.h>

TEST("x create action") {
  REQUIRE(!laplace::action {}.error());
}

TEST("x action default tick duration") {
  REQUIRE(laplace::action {}.tick_duration() ==
          laplace::action::default_tick_duration);
}

TEST("x action set tick duration") {
  REQUIRE(laplace::action {}.set_tick_duration(42).tick_duration() ==
          42);
}

TEST("x Set tick duration may fail") {
  REQUIRE(laplace::action {}.set_tick_duration(0).error());
  REQUIRE(laplace::action {}.set_tick_duration(-1).error());
}

TEST("x action run default") {
  auto run = laplace::action {}.run({});
  run.resume();
  REQUIRE(run.is_done());
}

TEST("x action setup and run") {
  bool called = false;

  auto run = laplace::action {}
                 .setup([&](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                   called = true;
                   co_return;
                 })
                 .run({});
  run.resume();

  REQUIRE(called);
}

TEST("x action propagate error") {
  REQUIRE(laplace::action {}
              .set_tick_duration(-1)
              .set_tick_duration(1)
              .error());
}

TEST("x action propagate error and run") {
  bool called = false;

  auto run = laplace::action {}
                 .set_tick_duration(-1)
                 .setup([&](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                   called = true;
                   co_return;
                 })
                 .run({});
  run.resume();

  REQUIRE(!called);
}

TEST("x action setup first, propagate error and run") {
  bool called = false;

  auto run = laplace::action {}
                 .setup([&](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                   called = true;
                   co_return;
                 })
                 .set_tick_duration(-1)
                 .run({});
  run.resume();

  REQUIRE(!called);
}
