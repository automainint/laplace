/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/state.h"
#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace {
  using std::make_shared, std::shared_ptr;

  TEST_CASE("create state") {
    std::ignore = state {};
  }

  TEST_CASE("state apply noop") {
    REQUIRE(state {}.apply(noop {}));
  }

  TEST_CASE("state apply may fail") {
    REQUIRE(!state {}.apply(tick_continue {}));
  }

  TEST_CASE("state get int may fail") {
    REQUIRE(state {}.get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("state apply allocate into int and get") {
    auto s  = state {};
    auto id = ptrdiff_t { 42 };
    REQUIRE(s.apply(integer_allocate_into { id, 1 }));
    REQUIRE(s.get_integer(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate int and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_allocate_into { ret, 1 }));
    REQUIRE(s.apply(integer_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.get_integer(id, 0, -1) == 0);
  }

  TEST_CASE("state apply reserve int and allocate into") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_reserve { 10 }));
    REQUIRE(s.apply(integer_allocate_into { id, 1 }));
    REQUIRE(s.get_integer(id, 0, -1) == 0);
  }

  TEST_CASE("state apply reserve int and allocate") {
    auto      s        = state {};
    ptrdiff_t ret      = 0;
    ptrdiff_t reserved = 10;
    REQUIRE(s.apply(integer_reserve { reserved }));
    REQUIRE(s.apply(integer_allocate_into { ret, 2 }));
    REQUIRE(s.apply(integer_allocate { 1, ret, 0 }));
    REQUIRE(s.apply(integer_allocate { 1, ret, 1 }));
    while (s.adjust()) { }
    REQUIRE(s.get_integer(ret, 0, -1) == reserved);
    REQUIRE(s.get_integer(ret, 1, -1) == reserved + 1);
  }

  TEST_CASE("state apply allocate int, set and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_allocate_into { ret, 1 }));
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
    REQUIRE(s.apply(integer_allocate_into { id, 1 }));
    REQUIRE(s.apply(integer_deallocate { id }));
  }

  TEST_CASE("state apply add int") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_allocate_into { id, 1 }));
    REQUIRE(s.apply(integer_add { id, 0, 18 }));
    REQUIRE(s.apply(integer_add { id, 0, 24 }));
    while (s.adjust()) { }
    REQUIRE(s.get_integer(id, 0, -1) == 42);
  }

  TEST_CASE("get byte may fail") {
    REQUIRE(state {}.get_byte(0, 0, -1) == -1);
  }

  TEST_CASE("state apply allocate into byte and get") {
    auto s  = state {};
    auto id = ptrdiff_t { 42 };
    REQUIRE(s.apply(byte_allocate_into { id, 1 }));
    REQUIRE(s.get_byte(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate byte and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_allocate_into { ret, 1 }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.get_byte(id, 0, -1) == 0);
  }

  TEST_CASE("state apply allocate byte, set and get") {
    auto s   = state {};
    auto ret = ptrdiff_t { 0 };
    REQUIRE(s.apply(integer_allocate_into { ret, 1 }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 0 }));
    while (s.adjust()) { }
    s.adjust_done();
    auto id = s.get_integer(ret, 0, -1);
    REQUIRE(id != -1);
    REQUIRE(s.apply(byte_set { id, 0, 42 }));
    while (s.adjust()) { }
    REQUIRE(s.get_byte(id, 0, -1) == 42);
  }

  TEST_CASE("state apply reserve byte and allocate into") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(byte_reserve { 10 }));
    REQUIRE(s.apply(byte_allocate_into { id, 1 }));
    REQUIRE(s.get_byte(id, 0, -1) == 0);
  }

  TEST_CASE("state apply reserve byte and allocate") {
    auto      s        = state {};
    ptrdiff_t ret      = 0;
    ptrdiff_t reserved = 10;
    REQUIRE(s.apply(integer_allocate_into { ret, 2 }));
    REQUIRE(s.apply(byte_reserve { reserved }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 0 }));
    REQUIRE(s.apply(byte_allocate { 1, ret, 1 }));
    while (s.adjust()) { }
    REQUIRE(s.get_integer(ret, 0, -1) == reserved);
    REQUIRE(s.get_integer(ret, 1, -1) == reserved + 1);
  }

  TEST_CASE("state deallocate byte") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(byte_allocate_into { id, 1 }));
    REQUIRE(s.apply(byte_deallocate { id }));
  }

  TEST_CASE("state apply add byte") {
    auto s  = state {};
    auto id = ptrdiff_t { 0 };
    REQUIRE(s.apply(byte_allocate_into { id, 1 }));
    REQUIRE(s.apply(byte_add { id, 0, 18 }));
    REQUIRE(s.apply(byte_add { id, 0, 24 }));
    while (s.adjust()) { }
    REQUIRE(s.get_byte(id, 0, -1) == 42);
  }

  TEST_CASE("state apply random") {
    auto s    = state {};
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(s.apply(integer_allocate_into { id, size }));
    REQUIRE(s.apply(integer_random { 1, 100, id, 0, size }));
    while (s.adjust()) { }
    for (auto i = ptrdiff_t {}; i < size; i++) {
      REQUIRE((s.get_integer(id, i, -1) >= 1 &&
               s.get_integer(id, i, -1) <= 100));
    }
  }

  TEST_CASE("state seed") {
    REQUIRE(state {}.apply(integer_seed { 42 }));
  }

  TEST_CASE("state random with equal seed") {
    auto foo  = state {};
    auto bar  = state {};
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(foo.apply(integer_seed { 42 }));
    REQUIRE(bar.apply(integer_seed { 42 }));
    REQUIRE(foo.apply(integer_allocate_into { id, size }));
    REQUIRE(bar.apply(integer_allocate_into { id, size }));
    REQUIRE(foo.apply(integer_random { 1, 100, id, 0, size }));
    REQUIRE(bar.apply(integer_random { 1, 100, id, 0, size }));
    while (foo.adjust()) { }
    while (bar.adjust()) { }
    for (auto i = ptrdiff_t {}; i < size; i++) {
      REQUIRE(
          (foo.get_integer(id, i, -1) == bar.get_integer(id, i, -1)));
    }
  }

  TEST_CASE("state random with different seed") {
    auto foo  = state {};
    auto bar  = state {};
    auto id   = ptrdiff_t { 0 };
    auto size = ptrdiff_t { 1000 };
    REQUIRE(foo.apply(integer_seed { 42 }));
    REQUIRE(bar.apply(integer_seed { 43 }));
    REQUIRE(foo.apply(integer_allocate_into { id, size }));
    REQUIRE(bar.apply(integer_allocate_into { id, size }));
    REQUIRE(foo.apply(integer_random { 1, 100, id, 0, size }));
    REQUIRE(bar.apply(integer_random { 1, 100, id, 0, size }));
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

    REQUIRE(s.apply(integer_allocate_into { id, 5 }));
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

  TEST_CASE("state copy and edit") {
    auto foo = state {};
    auto bar = foo;
    REQUIRE(foo.apply(integer_allocate_into { 0, 1 }));
    REQUIRE(foo.apply(integer_set { 0, 0, 1 }));
    while (foo.adjust()) { }
    REQUIRE(foo.get_integer(0, 0, -1) == 1);
    REQUIRE(bar.get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("state assign and edit") {
    auto foo = state {};
    auto bar = state {};
    bar      = foo;
    REQUIRE(foo.apply(integer_allocate_into { 0, 1 }));
    REQUIRE(foo.apply(integer_set { 0, 0, 1 }));
    while (foo.adjust()) { }
    REQUIRE(foo.get_integer(0, 0, -1) == 1);
    REQUIRE(bar.get_integer(0, 0, -1) == -1);
  }

  TEST_CASE("custom io implementation") {
    struct custom_io_impl : io_interface {
      [[nodiscard]] auto clone() const noexcept
          -> shared_ptr<io_interface> override {
        return make_shared<custom_io_impl>(*this);
      }
      [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                     int_type def) const noexcept
          -> int_type override {
        return 42;
      }
      [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                  byte_type def) const noexcept
          -> byte_type override {
        return 24;
      }
      [[nodiscard]] auto apply(impact const &i) noexcept
          -> bool override {
        return true;
      }
      [[nodiscard]] auto adjust() noexcept -> bool override {
        return false;
      }
      void adjust_done() noexcept override { }
    };

    auto s = state { make_shared<custom_io_impl>() };
    REQUIRE(s.get_integer(0, 0, -1) == 42);
    REQUIRE(s.get_byte(0, 0, -1) == 24);
    REQUIRE(s.apply(tick_continue {}));
    REQUIRE(!s.adjust());
  }
}
