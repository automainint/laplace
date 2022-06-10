/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/action.h"
#include <catch2/catch.hpp>

#include <iostream>

namespace laplace::test {
  TEST_CASE("Create action") {
    REQUIRE(!action {}.is_error());
  }

  TEST_CASE("Action default tick duration") {
    REQUIRE(action {}.get_tick_duration() ==
            action::default_tick_duration);
  }

  TEST_CASE("Action set tick duration") {
    REQUIRE(action {}.set_tick_duration(42).get_tick_duration() ==
            42);
  }

  TEST_CASE("Set tick duration may fail") {
    REQUIRE(action {}.set_tick_duration(0).is_error());
    REQUIRE(action {}.set_tick_duration(-1).is_error());
  }

  TEST_CASE("Action run default") {
    auto run = action {}.run();
    run.resume();
    REQUIRE(run.is_done());
  }
  
  TEST_CASE("Action setup and run") {
    bool called = false;

    auto run = action {}
                   .setup([&]() -> coro::generator<impact> {
                     called = true;
                     co_return;
                   })
                   .run();
    run.resume();

    REQUIRE(called);
  }

  TEST_CASE("Action propagate error") {
    REQUIRE(action {}
                .set_tick_duration(-1)
                .set_tick_duration(1)
                .is_error());
  }

  TEST_CASE("Action propagate error and run") {
    bool called = false;

    auto run = action {}
                   .set_tick_duration(-1)
                   .setup([&]() -> coro::generator<impact> {
                     called = true;
                     co_return;
                   })
                   .run();
    run.resume();

    REQUIRE(!called);
  }

  TEST_CASE("Action setup first, propagate error and run") {
    bool called = false;

    auto run = action {}
                   .setup([&]() -> coro::generator<impact> {
                     called = true;
                     co_return;
                   })
                   .set_tick_duration(-1)
                   .run();
    run.resume();

    REQUIRE(!called);
  }
}
