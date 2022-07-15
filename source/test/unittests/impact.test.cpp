#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("create impact") {
    std::ignore = impact { noop {} };
    std::ignore = impact { tick_continue {} };
    std::ignore = impact { integer_reserve { .count = 1 } };
    std::ignore = impact { integer_allocate_into { .id   = 0,
                                                   .size = 1 } };
    std::ignore = impact { integer_allocate {
        .size = 1, .return_id = 0, .return_index = 0 } };
    std::ignore = impact { integer_deallocate { .id = 0 } };
    std::ignore = impact { integer_set {
        .id = 0, .index = 0, .value = 0 } };
    std::ignore = impact { integer_add {
        .id = 0, .index = 0, .delta = 0 } };
    std::ignore = impact { byte_reserve { .count = 1 } };
    std::ignore = impact { byte_allocate_into { .id   = 0,
                                                .size = 1 } };
    std::ignore = impact { byte_allocate {
        .size = 1, .return_id = 0, .return_index = 0 } };
    std::ignore = impact { byte_deallocate { .id = 0 } };
    std::ignore = impact { byte_set {
        .id = 0, .index = 0, .value = 0 } };
    std::ignore = impact { byte_add {
        .id = 0, .index = 0, .delta = 0 } };
    std::ignore = impact { integer_seed { .seed = 42 } };
    std::ignore = impact { integer_random { .min          = 1,
                                            .max          = 100,
                                            .return_id    = 0,
                                            .return_index = 0,
                                            .return_size  = 1 } };
  }

  TEST_CASE("impact list from two") {
    impact_list foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo.size() == 2);
  }

  TEST_CASE("impact list from three") {
    impact_list foo = impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } } +
                      impact { tick_continue {} } +
                      impact { noop {} };
    REQUIRE(foo.size() == 3);
  }

  TEST_CASE("impact list single access") {
    impact_list foo = impact { integer_set {
        .id = 0, .index = 0, .value = 0 } };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
  }

  TEST_CASE("impact list small access") {
    impact_list foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
    REQUIRE(foo[1] == impact { tick_continue {} });
  }

  TEST_CASE("impact list add impact") {
    impact_list foo = impact_list { impact { noop {} } } +
                      impact { noop {} };
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { noop {} });
  }

  TEST_CASE("impact add impact list") {
    impact_list foo = impact { noop {} } +
                      impact_list { impact { noop {} } };
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { noop {} });
  }

  TEST_CASE("impact list add impact list") {
    impact_list foo =
        (impact { noop {} } + impact { tick_continue {} }) +
        (impact { tick_continue {} } + impact { noop {} });
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { tick_continue {} });
    REQUIRE(foo[2] == impact { tick_continue {} });
    REQUIRE(foo[3] == impact { noop {} });
  }

  TEST_CASE("impact mode") {
    REQUIRE(mode_of(noop {}) == mode::async);
    REQUIRE(mode_of(integer_set {}) == mode::async);
    REQUIRE(mode_of(integer_add {}) == mode::async);
    REQUIRE(mode_of(byte_set {}) == mode::async);
    REQUIRE(mode_of(byte_add {}) == mode::async);
    REQUIRE(mode_of(integer_reserve {}) == mode::sync);
    REQUIRE(mode_of(integer_allocate_into {}) == mode::sync);
    REQUIRE(mode_of(integer_allocate {}) == mode::sync);
    REQUIRE(mode_of(integer_deallocate {}) == mode::sync);
    REQUIRE(mode_of(byte_reserve {}) == mode::sync);
    REQUIRE(mode_of(byte_allocate_into {}) == mode::sync);
    REQUIRE(mode_of(byte_allocate {}) == mode::sync);
    REQUIRE(mode_of(byte_deallocate {}) == mode::sync);
    REQUIRE(mode_of(integer_seed {}) == mode::sync);
    REQUIRE(mode_of(integer_random {}) == mode::sync);
    REQUIRE(mode_of(queue_action {}) == mode::sync);
    REQUIRE(mode_of(tick_continue {}) == mode::control);
  }
}
