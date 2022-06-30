/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/action.h"
#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("create action") {
    REQUIRE(!action {}.error());
  }

  TEST_CASE("action default tick duration") {
    REQUIRE(action {}.tick_duration() ==
            action::default_tick_duration);
  }

  TEST_CASE("action set tick duration") {
    REQUIRE(action {}.set_tick_duration(42).tick_duration() == 42);
  }

  TEST_CASE("Set tick duration may fail") {
    REQUIRE(action {}.set_tick_duration(0).error());
    REQUIRE(action {}.set_tick_duration(-1).error());
  }

  TEST_CASE("action run default") {
    auto run = action {}.run({});
    run.resume();
    REQUIRE(run.is_done());
  }

  TEST_CASE("action setup and run") {
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

  TEST_CASE("action propagate error") {
    REQUIRE(
        action {}.set_tick_duration(-1).set_tick_duration(1).error());
  }

  TEST_CASE("action propagate error and run") {
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

  TEST_CASE("action setup first, propagate error and run") {
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
