/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/state.h"
#include <catch2/catch.hpp>

namespace laplace {
  TEST_CASE("create state") {
    REQUIRE(!state {}.is_error());
  }

  TEST_CASE("state apply noop") {
    REQUIRE(state {}.apply(noop {}));
  }

  TEST_CASE("state apply tick_continue will fail") {
    REQUIRE(!state {}.apply(tick_continue {}));
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

  TEST_CASE("get byte may fail") {
    REQUIRE(state {}.get_byte(0, 0, -1) == -1);
  }

  TEST_CASE("state apply reallocate byte and get") {
    auto s  = state {};
    auto id = ptrdiff_t { 42 };
    REQUIRE(s.apply(byte_reallocate { id, 1 }));
    REQUIRE(s.get_byte(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate byte and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { ret, 1 }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.get_byte(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate byte, set and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reallocate { ret, 1 }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    s.adjust_done();
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.apply(byte_set { id, 0, 42 }));
    while (s.adjust()) { }
    REQUIRE(s.get_byte(id, 0, -1) == 42);
  }

  TEST_CASE("state deallocate byte") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(byte_reallocate { id, 1 }));
    REQUIRE(s.apply(byte_deallocate { id }));
  }

  TEST_CASE("state apply add byte") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(byte_reallocate { id, 1 }));
    REQUIRE(s.apply(byte_add { id, 0, 18 }));
    REQUIRE(s.apply(byte_add { id, 0, 24 }));
    while (s.adjust()) { }
    REQUIRE(s.get_byte(id, 0, -1) == 42);
  }

  TEST_CASE("state apply random") {
    auto s    = state {};
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(s.apply(integer_reallocate { id, size }));
    REQUIRE(s.apply(random { 1, 100, id, 0, size }));
    while (s.adjust()) { }
    for (auto i = ptrdiff_t {}; i < size; i++) {
      REQUIRE((s.get_integer(id, i, -1) >= 1 &&
               s.get_integer(id, i, -1) <= 100));
    }
  }

  TEST_CASE("state seed") {
    REQUIRE(!state {}.seed(42).is_error());
  }

  TEST_CASE("state random with equal seed") {
    auto foo  = state {}.seed(42);
    auto bar  = state {}.seed(42);
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(foo.apply(integer_reallocate { id, size }));
    REQUIRE(bar.apply(integer_reallocate { id, size }));
    REQUIRE(foo.apply(random { 1, 100, id, 0, size }));
    REQUIRE(bar.apply(random { 1, 100, id, 0, size }));
    while (foo.adjust()) { }
    while (bar.adjust()) { }
    for (auto i = ptrdiff_t {}; i < size; i++) {
      REQUIRE(
          (foo.get_integer(id, i, -1) == bar.get_integer(id, i, -1)));
    }
  }

  TEST_CASE("state random with different seed") {
    auto foo  = state {}.seed(42);
    auto bar  = state {}.seed(43);
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(foo.apply(integer_reallocate { id, size }));
    REQUIRE(bar.apply(integer_reallocate { id, size }));
    REQUIRE(foo.apply(random { 1, 100, id, 0, size }));
    REQUIRE(bar.apply(random { 1, 100, id, 0, size }));
    while (foo.adjust()) { }
    while (bar.adjust()) { }

    auto different_count = ptrdiff_t {};
    for (auto i = ptrdiff_t {}; i < size; i++)
      if ((foo.get_integer(id, i, -1) != bar.get_integer(id, i, -1)))
        different_count++;

    REQUIRE(different_count > size / 2);
  }

  TEST_CASE("state allocation determinism") {
    auto s  = state {};
    auto id = ptrdiff_t {};

    REQUIRE(s.apply(integer_reallocate { id, 5 }));
    REQUIRE(s.apply(integer_allocate { 10, id, 0 }));
    REQUIRE(s.apply(integer_allocate { 20, id, 1 }));
    REQUIRE(s.apply(integer_allocate { 30, id, 2 }));

    while (s.adjust()) { }
    s.adjust_done();

    REQUIRE(s.get_integer(id, 0, -1) == 1);
    REQUIRE(s.get_integer(id, 1, -1) == 2);
    REQUIRE(s.get_integer(id, 2, -1) == 3);

    REQUIRE(s.apply(integer_deallocate { s.get_integer(id, 0, -1) }));
    REQUIRE(s.apply(integer_allocate { 20, id, 3 }));
    REQUIRE(s.apply(integer_allocate { 30, id, 4 }));

    while (s.adjust()) { }
    s.adjust_done();

    REQUIRE(s.get_integer(id, 3, -1) == 1);
    REQUIRE(s.get_integer(id, 4, -1) == 4);

    REQUIRE(s.apply(integer_deallocate { s.get_integer(id, 2, -1) }));
    REQUIRE(s.apply(integer_deallocate { s.get_integer(id, 3, -1) }));
    REQUIRE(s.apply(integer_allocate { 20, id, 0 }));

    while (s.adjust()) { }
    s.adjust_done();

    REQUIRE(s.get_integer(id, 0, -1) == 1);
  }
}
