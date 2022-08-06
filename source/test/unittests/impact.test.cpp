#include "../../laplace/impact.h"

#define KIT_TEST_FILE impact
#include <kit_test/test.h>

TEST("create impact") {
  std::ignore = laplace::impact { laplace::noop {} };
  std::ignore = laplace::impact { laplace::tick_continue {} };
  std::ignore = laplace::impact { laplace::integer_reserve {
      .count = 1 } };
  std::ignore = laplace::impact { laplace::integer_allocate_into {
      .id = 0, .size = 1 } };
  std::ignore = laplace::impact { laplace::integer_allocate {
      .size = 1, .return_id = 0, .return_index = 0 } };
  std::ignore = laplace::impact { laplace::integer_deallocate {
      .id = 0 } };
  std::ignore = laplace::impact { laplace::integer_set {
      .id = 0, .index = 0, .value = 0 } };
  std::ignore = laplace::impact { laplace::integer_add {
      .id = 0, .index = 0, .delta = 0 } };
  std::ignore = laplace::impact { laplace::byte_reserve {
      .count = 1 } };
  std::ignore = laplace::impact { laplace::byte_allocate_into {
      .id = 0, .size = 1 } };
  std::ignore = laplace::impact { laplace::byte_allocate {
      .size = 1, .return_id = 0, .return_index = 0 } };
  std::ignore = laplace::impact { laplace::byte_deallocate {
      .id = 0 } };
  std::ignore = laplace::impact { laplace::byte_set {
      .id = 0, .index = 0, .value = 0 } };
  std::ignore = laplace::impact { laplace::byte_add {
      .id = 0, .index = 0, .delta = 0 } };
  std::ignore = laplace::impact { laplace::integer_seed {
      .seed = 42 } };
  std::ignore = laplace::impact { laplace::integer_random {
      .min          = 1,
      .max          = 100,
      .return_id    = 0,
      .return_index = 0,
      .return_size  = 1 } };
}

TEST("impact list from two") {
  laplace::impact_list foo = laplace::impact {
    laplace::integer_set { .id = 0, .index = 0, .value = 0 }
  } + laplace::impact { laplace::tick_continue {} };
  REQUIRE(foo.size() == 2);
}

TEST("impact list from three") {
  laplace::impact_list foo = laplace::impact { laplace::integer_set {
                                 .id = 0, .index = 0, .value = 0 } } +
                             laplace::impact {
                               laplace::tick_continue {}
                             } +
                             laplace::impact { laplace::noop {} };
  REQUIRE(foo.size() == 3);
}

TEST("impact list single access") {
  laplace::impact_list foo = laplace::impact { laplace::integer_set {
      .id = 0, .index = 0, .value = 0 } };
  REQUIRE(foo[0] == laplace::impact { laplace::integer_set {
                        .id = 0, .index = 0, .value = 0 } });
}

TEST("impact list small access") {
  laplace::impact_list foo = laplace::impact {
    laplace::integer_set { .id = 0, .index = 0, .value = 0 }
  } + laplace::impact { laplace::tick_continue {} };
  REQUIRE(foo[0] == laplace::impact { laplace::integer_set {
                        .id = 0, .index = 0, .value = 0 } });
  REQUIRE(foo[1] == laplace::impact { laplace::tick_continue {} });
}

TEST("impact list add impact") {
  laplace::impact_list foo = laplace::impact_list {
    laplace::impact { laplace::noop {} }
  } + laplace::impact { laplace::noop {} };
  REQUIRE(foo[0] == laplace::impact { laplace::noop {} });
  REQUIRE(foo[1] == laplace::impact { laplace::noop {} });
}

TEST("impact add impact list") {
  laplace::impact_list foo = laplace::impact {
    laplace::noop {}
  } + laplace::impact_list { laplace::impact { laplace::noop {} } };
  REQUIRE(foo[0] == laplace::impact { laplace::noop {} });
  REQUIRE(foo[1] == laplace::impact { laplace::noop {} });
}

TEST("impact list add impact list") {
  laplace::impact_list foo =
      (laplace::impact { laplace::noop {} } +
       laplace::impact { laplace::tick_continue {} }) +
      (laplace::impact { laplace::tick_continue {} } +
       laplace::impact { laplace::noop {} });
  REQUIRE(foo[0] == laplace::impact { laplace::noop {} });
  REQUIRE(foo[1] == laplace::impact { laplace::tick_continue {} });
  REQUIRE(foo[2] == laplace::impact { laplace::tick_continue {} });
  REQUIRE(foo[3] == laplace::impact { laplace::noop {} });
}

TEST("impact mode") {
  REQUIRE(mode_of(laplace::impact { laplace::noop {} }) ==
          laplace::mode::async);
  REQUIRE(mode_of(laplace::impact { laplace::integer_set {} }) ==
          laplace::mode::async);
  REQUIRE(mode_of(laplace::impact { laplace::integer_add {} }) ==
          laplace::mode::async);
  REQUIRE(mode_of(laplace::impact { laplace::byte_set {} }) ==
          laplace::mode::async);
  REQUIRE(mode_of(laplace::impact { laplace::byte_add {} }) ==
          laplace::mode::async);
  REQUIRE(mode_of(laplace::impact { laplace::integer_reserve {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact {
              laplace::integer_allocate_into {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::integer_allocate {} }) ==
          laplace::mode::sync);
  REQUIRE(
      mode_of(laplace::impact { laplace::integer_deallocate {} }) ==
      laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::byte_reserve {} }) ==
          laplace::mode::sync);
  REQUIRE(
      mode_of(laplace::impact { laplace::byte_allocate_into {} }) ==
      laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::byte_allocate {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::byte_deallocate {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::integer_seed {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::integer_random {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::queue_action {} }) ==
          laplace::mode::sync);
  REQUIRE(mode_of(laplace::impact { laplace::tick_continue {} }) ==
          laplace::mode::control);
}
