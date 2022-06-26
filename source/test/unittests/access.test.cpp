/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/access.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("create access") {
    std::ignore = access {};
  }

  TEST_CASE("access get will fail") {
    REQUIRE(access {}.get_integer(0, 0, -1) == -1);
    REQUIRE(access {}.get_byte(0, 0, -1) == -1);
  }

  TEST_CASE("create access from state") {
    auto s      = state {};
    std::ignore = access { s };
  }

  TEST_CASE("access get from state") {
    auto s      = state {};
    std::ignore = s.apply(integer_reallocate { 0, 1 });
    std::ignore = s.apply(integer_set { 0, 0, 42 });
    std::ignore = s.apply(byte_reallocate { 1, 1 });
    std::ignore = s.apply(byte_set { 1, 0, 43 });
    while (s.adjust()) { }

    auto a = access { s };
    REQUIRE(a.get_integer(0, 0, -1) == 42);
    REQUIRE(a.get_byte(1, 0, -1) == 43);
  }
}
