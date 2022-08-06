#include "../../laplace/access.h"
#include "../../laplace/impact.h"

#define KIT_TEST_FILE access
#include <kit_test/test.h>

TEST("create access") {
  std::ignore = laplace::access {};
}

TEST("access get will fail") {
  REQUIRE(laplace::access {}.get_integer(0, 0, -1) == -1);
  REQUIRE(laplace::access {}.get_byte(0, 0, -1) == -1);
}

TEST("create access from state") {
  auto s      = laplace::state {};
  std::ignore = laplace::access { s };
}

TEST("access get from state") {
  auto s      = laplace::state {};
  std::ignore = s.apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } });
  std::ignore = s.apply(
      laplace::impact { laplace::integer_set { 0, 0, 42 } });
  std::ignore = s.apply(
      laplace::impact { laplace::byte_allocate_into { 1, 1 } });
  std::ignore = s.apply(
      laplace::impact { laplace::byte_set { 1, 0, 43 } });
  while (s.adjust()) { }

  auto a = laplace::access { s };
  REQUIRE(a.get_integer(0, 0, -1) == 42);
  REQUIRE(a.get_byte(1, 0, -1) == 43);
}

TEST("access get from destroyed state") {
  auto *s     = new laplace::state {};
  std::ignore = s->apply(
      laplace::impact { laplace::integer_allocate_into { 0, 1 } });
  std::ignore = s->apply(
      laplace::impact { laplace::integer_set { 0, 0, 42 } });
  std::ignore = s->apply(
      laplace::impact { laplace::byte_allocate_into { 1, 1 } });
  std::ignore = s->apply(
      laplace::impact { laplace::byte_set { 1, 0, 43 } });
  while (s->adjust()) { }

  auto a = laplace::access { *s };
  delete s;

  REQUIRE(a.get_integer(0, 0, -1) == 42);
  REQUIRE(a.get_byte(1, 0, -1) == 43);
}
