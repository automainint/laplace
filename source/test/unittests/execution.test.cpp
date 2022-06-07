/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/execution.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("Create execution", "[execution]") {
    REQUIRE(!execution {}.is_error());
  }

  TEST_CASE("Can get thread count", "[execution]") {
    REQUIRE(execution {}.get_thread_count() ==
            execution::default_thread_count);
  }

  TEST_CASE("Can set thread count", "[execution]") {
    auto exe = execution {}.set_thread_count(4);
    REQUIRE(!exe.is_error());
    REQUIRE(exe.get_thread_count() == 4);
  }

  TEST_CASE("Can set thread count to zero", "[execution]") {
    REQUIRE(!execution {}.set_thread_count(0).is_error());
  }

  TEST_CASE("May fail to set thread count", "[execution]") {
    auto exe = execution {};
    REQUIRE(exe.set_thread_count(-1).is_error());
    REQUIRE(exe.set_thread_count(10000000000).is_error());
    REQUIRE(exe.set_thread_count(10000).is_error());
  }

  TEST_CASE("Can propagate error", "[execution]") {
    REQUIRE(execution {}
                .set_thread_count(-1)
                .set_thread_count(0)
                .is_error());
  }

  TEST_CASE("Can spawn static entity", "[execution]") { }

  TEST_CASE("Can spawn dynamic entity", "[execution]") { }

  TEST_CASE("Can queue impact", "[execution]") { }
}
