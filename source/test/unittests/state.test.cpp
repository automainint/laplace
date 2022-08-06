#include "../../laplace/state.h"
#include "../../laplace/impact.h"
#include "io_impl.test.h"

#define KIT_TEST_FILE state
#include <kit_test/test.h>

using std::make_shared;

TEST("create state") {
  std::ignore = laplace::state {};
}

TEST("state apply noop") {
  REQUIRE(
      laplace::state {}.apply(laplace::impact { laplace::noop {} }));
}

TEST("state apply may fail") {
  REQUIRE(!laplace::state {}.apply(
      laplace::impact { laplace::tick_continue {} }));
}

TEST("state get int may fail") {
  REQUIRE(laplace::state {}.get_integer(0, 0, -1) == -1);
}

TEST("state apply allocate into int and get") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 42 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { id, 1 } }));
  REQUIRE(s.get_integer(id, 0, -1) == 0);
}

TEST("state apply allocate int and get") {
  auto s   = laplace::state {};
  auto ret = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 1, ret, 0 } }));
  while (s.adjust()) { }
  auto id = s.get_integer(ret, 0, -1);
  REQUIRE(id != -1);
  REQUIRE(s.get_integer(id, 0, -1) == 0);
}

TEST("state apply reserve int and allocate into") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(
      s.apply(laplace::impact { laplace::integer_reserve { 10 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { id, 1 } }));
  REQUIRE(s.get_integer(id, 0, -1) == 0);
}

TEST("state apply reserve int and allocate") {
  auto      s        = laplace::state {};
  ptrdiff_t ret      = 0;
  ptrdiff_t reserved = 10;
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_reserve { reserved } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 2 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 1, ret, 0 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 1, ret, 1 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_integer(ret, 0, -1) == reserved);
  REQUIRE(s.get_integer(ret, 1, -1) == reserved + 1);
}

TEST("state apply allocate int, set and get") {
  auto s   = laplace::state {};
  auto ret = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 1, ret, 0 } }));
  while (s.adjust()) { }
  s.adjust_done();
  auto id = s.get_integer(ret, 0, -1);
  REQUIRE(id != -1);
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_set { id, 0, 42 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_integer(id, 0, -1) == 42);
}

TEST("state deallocate int") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { id, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_deallocate { id } }));
}

TEST("state apply add int") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { id, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_add { id, 0, 18 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_add { id, 0, 24 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_integer(id, 0, -1) == 42);
}

TEST("get byte may fail") {
  REQUIRE(laplace::state {}.get_byte(0, 0, -1) == -1);
}

TEST("state apply allocate into byte and get") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 42 };
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate_into { id, 1 } }));
  REQUIRE(s.get_byte(id, 0, -1) == 0);
}

TEST("state apply allocate byte and get") {
  auto s   = laplace::state {};
  auto ret = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate { 1, ret, 0 } }));
  while (s.adjust()) { }
  auto id = s.get_integer(ret, 0, -1);
  REQUIRE(id != -1);
  REQUIRE(s.get_byte(id, 0, -1) == 0);
}

TEST("state apply allocate byte, set and get") {
  auto s   = laplace::state {};
  auto ret = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate { 1, ret, 0 } }));
  while (s.adjust()) { }
  s.adjust_done();
  auto id = s.get_integer(ret, 0, -1);
  REQUIRE(id != -1);
  REQUIRE(
      s.apply(laplace::impact { laplace::byte_set { id, 0, 42 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_byte(id, 0, -1) == 42);
}

TEST("state apply reserve byte and allocate into") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(s.apply(laplace::impact { laplace::byte_reserve { 10 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate_into { id, 1 } }));
  REQUIRE(s.get_byte(id, 0, -1) == 0);
}

TEST("state apply reserve byte and allocate") {
  auto      s        = laplace::state {};
  ptrdiff_t ret      = 0;
  ptrdiff_t reserved = 10;
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { ret, 2 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_reserve { reserved } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate { 1, ret, 0 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate { 1, ret, 1 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_integer(ret, 0, -1) == reserved);
  REQUIRE(s.get_integer(ret, 1, -1) == reserved + 1);
}

TEST("state deallocate byte") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate_into { id, 1 } }));
  REQUIRE(
      s.apply(laplace::impact { laplace::byte_deallocate { id } }));
}

TEST("state apply add byte") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t { 0 };
  REQUIRE(s.apply(
      laplace::impact { laplace::byte_allocate_into { id, 1 } }));
  REQUIRE(
      s.apply(laplace::impact { laplace::byte_add { id, 0, 18 } }));
  REQUIRE(
      s.apply(laplace::impact { laplace::byte_add { id, 0, 24 } }));
  while (s.adjust()) { }
  REQUIRE(s.get_byte(id, 0, -1) == 42);
}

TEST("state apply random") {
  auto s    = laplace::state {};
  auto id   = ptrdiff_t { 0 };
  auto size = ptrdiff_t { 1000 };
  REQUIRE(s.apply(laplace::impact {
      laplace::integer_allocate_into { id, size } }));
  REQUIRE(s.apply(laplace::impact {
      laplace::integer_random { 1, 100, id, 0, size } }));
  while (s.adjust()) { }
  bool ok = true;
  for (auto i = ptrdiff_t {}; i < size; i++)
    ok = ok && ((s.get_integer(id, i, -1) >= 1 &&
                 s.get_integer(id, i, -1) <= 100));
  REQUIRE(ok);
}

TEST("state apply seed") {
  REQUIRE(laplace::state {}.apply(
      laplace::impact { laplace::integer_seed { 42 } }));
}

TEST("state random with equal seed") {
  auto foo  = laplace::state {};
  auto bar  = laplace::state {};
  auto id   = ptrdiff_t { 0 };
  auto size = ptrdiff_t { 1000 };
  REQUIRE(
      foo.apply(laplace::impact { laplace::integer_seed { 42 } }));
  REQUIRE(
      bar.apply(laplace::impact { laplace::integer_seed { 42 } }));
  REQUIRE(foo.apply(laplace::impact {
      laplace::integer_allocate_into { id, size } }));
  REQUIRE(bar.apply(laplace::impact {
      laplace::integer_allocate_into { id, size } }));
  REQUIRE(foo.apply(laplace::impact {
      laplace::integer_random { 1, 100, id, 0, size } }));
  REQUIRE(bar.apply(laplace::impact {
      laplace::integer_random { 1, 100, id, 0, size } }));
  while (foo.adjust()) { }
  while (bar.adjust()) { }
  bool ok = true;
  for (auto i = ptrdiff_t {}; i < size; i++)
    ok = ok &&
         foo.get_integer(id, i, -1) == bar.get_integer(id, i, -1);
  REQUIRE(ok);
}

TEST("state random with different seed") {
  auto foo  = laplace::state {};
  auto bar  = laplace::state {};
  auto id   = ptrdiff_t { 0 };
  auto size = ptrdiff_t { 1000 };
  REQUIRE(
      foo.apply(laplace::impact { laplace::integer_seed { 42 } }));
  REQUIRE(
      bar.apply(laplace::impact { laplace::integer_seed { 43 } }));
  REQUIRE(foo.apply(laplace::impact {
      laplace::integer_allocate_into { id, size } }));
  REQUIRE(bar.apply(laplace::impact {
      laplace::integer_allocate_into { id, size } }));
  REQUIRE(foo.apply(laplace::impact {
      laplace::integer_random { 1, 100, id, 0, size } }));
  REQUIRE(bar.apply(laplace::impact {
      laplace::integer_random { 1, 100, id, 0, size } }));
  while (foo.adjust()) { }
  while (bar.adjust()) { }

  auto different_count = ptrdiff_t {};
  for (auto i = ptrdiff_t {}; i < size; i++)
    if ((foo.get_integer(id, i, -1) != bar.get_integer(id, i, -1)))
      different_count++;

  REQUIRE(different_count > size / 2);
}

TEST("state allocation determinism") {
  auto s  = laplace::state {};
  auto id = ptrdiff_t {};

  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate_into { id, 5 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 10, id, 0 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 20, id, 1 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 30, id, 2 } }));

  while (s.adjust()) { }
  s.adjust_done();

  REQUIRE(s.get_integer(id, 0, -1) == 1);
  REQUIRE(s.get_integer(id, 1, -1) == 2);
  REQUIRE(s.get_integer(id, 2, -1) == 3);

  REQUIRE(s.apply(laplace::impact {
      laplace::integer_deallocate { s.get_integer(id, 0, -1) } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 20, id, 3 } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 30, id, 4 } }));

  while (s.adjust()) { }
  s.adjust_done();

  REQUIRE(s.get_integer(id, 3, -1) == 1);
  REQUIRE(s.get_integer(id, 4, -1) == 4);

  REQUIRE(s.apply(laplace::impact {
      laplace::integer_deallocate { s.get_integer(id, 2, -1) } }));
  REQUIRE(s.apply(laplace::impact {
      laplace::integer_deallocate { s.get_integer(id, 3, -1) } }));
  REQUIRE(s.apply(
      laplace::impact { laplace::integer_allocate { 20, id, 0 } }));

  while (s.adjust()) { }
  s.adjust_done();

  REQUIRE(s.get_integer(id, 0, -1) == 1);
}

TEST("state copy and edit") {
  auto foo = laplace::state {};
  auto bar = foo;
  REQUIRE(foo.apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } }));
  REQUIRE(foo.apply(
      laplace::impact { laplace::integer_set { 0, 0, 1 } }));
  while (foo.adjust()) { }
  REQUIRE(foo.get_integer(0, 0, -1) == 1);
  REQUIRE(bar.get_integer(0, 0, -1) == -1);
}

TEST("state assign and edit") {
  auto foo = laplace::state {};
  auto bar = laplace::state {};
  bar      = foo;
  REQUIRE(foo.apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } }));
  REQUIRE(foo.apply(
      laplace::impact { laplace::integer_set { 0, 0, 1 } }));
  while (foo.adjust()) { }
  REQUIRE(foo.get_integer(0, 0, -1) == 1);
  REQUIRE(bar.get_integer(0, 0, -1) == -1);
}

TEST("custom io implementation") {
  auto s = laplace::state {
    make_shared<laplace::test::test_io_impl>()
  };
  REQUIRE(s.get_integer(0, 0, -1) == 42);
  REQUIRE(s.get_byte(0, 0, -1) == 24);
  REQUIRE(s.apply(laplace::impact { laplace::tick_continue {} }));
  REQUIRE(!s.adjust());
}
