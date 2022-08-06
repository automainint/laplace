#include "../../laplace/impact.h"

#define KIT_TEST_FILE impact
#include <kit_test/test.h>

namespace laplace::test {
  TEST("create impact") {
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

  TEST("impact list from two") {
    impact_list foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo.size() == 2);
  }

  TEST("impact list from three") {
    impact_list foo = impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } } +
                      impact { tick_continue {} } +
                      impact { noop {} };
    REQUIRE(foo.size() == 3);
  }

  TEST("impact list single access") {
    impact_list foo = impact { integer_set {
        .id = 0, .index = 0, .value = 0 } };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
  }

  TEST("impact list small access") {
    impact_list foo = impact {
      integer_set { .id = 0, .index = 0, .value = 0 }
    } + impact { tick_continue {} };
    REQUIRE(foo[0] == impact { integer_set {
                          .id = 0, .index = 0, .value = 0 } });
    REQUIRE(foo[1] == impact { tick_continue {} });
  }

  TEST("impact list add impact") {
    impact_list foo = impact_list { impact { noop {} } } +
                      impact { noop {} };
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { noop {} });
  }

  TEST("impact add impact list") {
    impact_list foo = impact { noop {} } +
                      impact_list { impact { noop {} } };
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { noop {} });
  }

  TEST("impact list add impact list") {
    impact_list foo =
        (impact { noop {} } + impact { tick_continue {} }) +
        (impact { tick_continue {} } + impact { noop {} });
    REQUIRE(foo[0] == impact { noop {} });
    REQUIRE(foo[1] == impact { tick_continue {} });
    REQUIRE(foo[2] == impact { tick_continue {} });
    REQUIRE(foo[3] == impact { noop {} });
  }

  TEST("impact mode") {
    REQUIRE(mode_of(impact { noop {} }) == mode::async);
    REQUIRE(mode_of(impact { integer_set {} }) == mode::async);
    REQUIRE(mode_of(impact { integer_add {} }) == mode::async);
    REQUIRE(mode_of(impact { byte_set {} }) == mode::async);
    REQUIRE(mode_of(impact { byte_add {} }) == mode::async);
    REQUIRE(mode_of(impact { integer_reserve {} }) == mode::sync);
    REQUIRE(mode_of(impact { integer_allocate_into {} }) ==
            mode::sync);
    REQUIRE(mode_of(impact { integer_allocate {} }) == mode::sync);
    REQUIRE(mode_of(impact { integer_deallocate {} }) == mode::sync);
    REQUIRE(mode_of(impact { byte_reserve {} }) == mode::sync);
    REQUIRE(mode_of(impact { byte_allocate_into {} }) == mode::sync);
    REQUIRE(mode_of(impact { byte_allocate {} }) == mode::sync);
    REQUIRE(mode_of(impact { byte_deallocate {} }) == mode::sync);
    REQUIRE(mode_of(impact { integer_seed {} }) == mode::sync);
    REQUIRE(mode_of(impact { integer_random {} }) == mode::sync);
    REQUIRE(mode_of(impact { queue_action {} }) == mode::sync);
    REQUIRE(mode_of(impact { tick_continue {} }) == mode::control);
  }
}
