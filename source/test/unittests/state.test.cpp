/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/state.h"
#include <catch2/catch.hpp>

namespace laplace {
  TEST_CASE("create state") {
    REQUIRE(!state {}.is_error());
  }

  TEST_CASE("state apply noop") {
    auto s = state {};
    REQUIRE(s.apply(noop {}));
  }

  TEST_CASE("state get int may fail") {
    REQUIRE(state {}.get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("state apply reallocate int and get") {
    auto s  = state {};
    auto id = ptrdiff_t { 42 };
    REQUIRE(s.apply(integer_reallocate { id, 1 }));
    REQUIRE(s.get_integer(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate int and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { ret, 1 }));
    REQUIRE(s.apply(integer_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.get_integer(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate int, set and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { ret, 1 }));
    REQUIRE(s.apply(integer_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    s.adjust_done();
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.apply(integer_set { id, 0, 42 }));
    while (s.adjust()) { }
    REQUIRE(s.get_integer(id, 0, -1) == 42);
  }

  TEST_CASE("state deallocate int") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { id, 1 }));
    REQUIRE(s.apply(integer_deallocate { id }));
  }

  TEST_CASE("state apply add int") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { id, 1 }));
    REQUIRE(s.apply(integer_add { id, 0, 18 }));
    REQUIRE(s.apply(integer_add { id, 0, 24 }));
    while (s.adjust()) { }
    REQUIRE(s.get_integer(id, 0, -1) == 42);
  }
}
