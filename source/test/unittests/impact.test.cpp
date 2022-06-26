/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("create impact") {
    std::ignore = impact { noop {} };
    std::ignore = impact { tick_continue {} };
    std::ignore = impact { integer_reserve { .count = 1 } };
    std::ignore = impact { integer_reallocate { .id   = 0,
                                                .size = 1 } };
    std::ignore = impact { integer_allocate {
        .size = 1, .return_id = 0, .return_index = 0 } };
    std::ignore = impact { integer_deallocate { .id = 0 } };
    std::ignore = impact { integer_set {
        .id = 0, .index = 0, .value = 0 } };
    std::ignore = impact { integer_add {
        .id = 0, .index = 0, .delta = 0 } };
    std::ignore = impact { byte_reserve { .count = 1 } };
    std::ignore = impact { byte_reallocate { .id = 0, .size = 1 } };
    std::ignore = impact { byte_allocate {
        .size = 1, .return_id = 0, .return_index = 0 } };
    std::ignore = impact { byte_deallocate { .id = 0 } };
    std::ignore = impact { byte_set {
        .id = 0, .index = 0, .value = 0 } };
    std::ignore = impact { byte_add {
        .id = 0, .index = 0, .delta = 0 } };
    std::ignore = impact { random { .min          = 1,
                                    .max          = 100,
                                    .return_id    = 0,
                                    .return_index = 0,
                                    .return_size  = 1 } };
  }

  TEST_CASE("impact list") {
    auto foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo.get_size() == 2);
  }

  TEST_CASE("impact list single access") {
    auto foo = impact_list { impact {
        integer_set { .id = 0, .index = 0, .value = 0 } } };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
  }

  TEST_CASE("impact list small access") {
    auto foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
    REQUIRE(foo[1] == impact { tick_continue {} });
  }
}
