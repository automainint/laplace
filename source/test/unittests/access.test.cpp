#include "../../laplace/access.h"
#include "../../laplace/impact.h"

#define KIT_TEST_FILE access
#include <kit_test/test.h>

namespace laplace::test {
  TEST("create access") {
    std::ignore = access {};
  }

  TEST("access get will fail") {
    REQUIRE(access {}.get_integer(0, 0, -1) == -1);
    REQUIRE(access {}.get_byte(0, 0, -1) == -1);
  }

  TEST("create access from state") {
    auto s      = state {};
    std::ignore = access { s };
  }

  TEST("access get from state") {
    auto s      = state {};
    std::ignore = s.apply(impact { integer_allocate_into { 0, 1 } });
    std::ignore = s.apply(impact { integer_set { 0, 0, 42 } });
    std::ignore = s.apply(impact { byte_allocate_into { 1, 1 } });
    std::ignore = s.apply(impact { byte_set { 1, 0, 43 } });
    while (s.adjust()) { }

    auto a = access { s };
    REQUIRE(a.get_integer(0, 0, -1) == 42);
    REQUIRE(a.get_byte(1, 0, -1) == 43);
  }

  TEST("access get from destroyed state") {
    auto *s     = new state {};
    std::ignore = s->apply(impact { integer_allocate_into { 0, 1 } });
    std::ignore = s->apply(impact { integer_set { 0, 0, 42 } });
    std::ignore = s->apply(impact { byte_allocate_into { 1, 1 } });
    std::ignore = s->apply(impact { byte_set { 1, 0, 43 } });
    while (s->adjust()) { }

    auto a = access { *s };
    delete s;

    REQUIRE(a.get_integer(0, 0, -1) == 42);
    REQUIRE(a.get_byte(1, 0, -1) == 43);
  }

  /*TEST("access use in constexpr context") {
    constexpr auto n = []() {
      auto s = access {};
      return 0;
    }();

    REQUIRE(n == 0);
  }*/
}
