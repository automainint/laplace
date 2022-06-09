/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/execution.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("Create execution") {
    REQUIRE(!execution {}.is_error());
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
}
