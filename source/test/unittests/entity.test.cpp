#include "../../laplace/entity.h"
#include "../../laplace/impact.h"

#define KIT_TEST_FILE entity
#include <kit_test/test.h>

using std::numeric_limits;

TEST("create entity") {
  REQUIRE(!laplace::entity {}.error());
}

TEST("entity size is zero by default") {
  REQUIRE(laplace::entity {}.size() == 0);
}

TEST("entity default id") {
  REQUIRE(laplace::entity {}.id() == laplace::id_undefined);
}

TEST("entity default access") {
  REQUIRE(laplace::entity {}.access() == laplace::access {});
}

TEST("entity set id") {
  REQUIRE(laplace::entity {}.set_id(42).id() == 42);
}

TEST("entity set access") {
  auto s = laplace::state {};
  REQUIRE(
      laplace::entity {}.set_access(laplace::access { s }).access() ==
      laplace::access { s });
}

TEST("entity setup fields") {
  REQUIRE(laplace::entity {}.setup({}).size() == 0);
}

TEST("entity setup 3 fields") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 }, { .id = 3 } })
              .size() == 3);
}

TEST("entity setup twice") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .setup({ { .id = 3 }, { .id = 4 }, { .id = 5 } })
              .size() == 5);
}

TEST("entity setup and get index") {
  REQUIRE(laplace::entity {}.setup({ { .id = 42 } }).index_of(42) ==
          0);
}

TEST("entity setup 2 fields and get index") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 42 } })
              .index_of(42) == 1);
}

TEST("entity get index may fail") {
  REQUIRE(laplace::entity {}.index_of(42) ==
          laplace::index_undefined);
}

TEST("entity setup unsorted") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 3 }, { .id = 2 }, { .id = 1 } })
              .index_of(1) == 0);
}

TEST("entity setup repeating will fail") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 }, { .id = 1 } })
              .error());
}

TEST("entity negative field ids not allowed") {
  REQUIRE(laplace::entity {}.setup({ { .id = -3 } }).error());
}

TEST("entity setup twice with repeating") {
  auto e = laplace::entity {}
               .setup({ { .id = 1 }, { .id = 2 } })
               .setup({ { .id = 1 }, { .id = 3 }, { .id = 4 } });
  REQUIRE(!e.error());
  REQUIRE(e.size() == 4);
  REQUIRE(e.index_of(1) == 0);
  REQUIRE(e.index_of(2) == 1);
  REQUIRE(e.index_of(3) == 2);
  REQUIRE(e.index_of(4) == 3);
}

TEST("entity propagate error") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 1 } })
              .setup({ { .id = 42 } })
              .error());
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 1 } })
              .set_id(0)
              .error());
}

TEST("entity spawn with id") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .set_id(42)
              .spawn() ==
          laplace::impact { laplace::integer_allocate_into {
              .id = 42, .size = 2 } });
}

TEST("entity remove") {
  REQUIRE(
      laplace::entity {}.set_id(42).remove() ==
      laplace::impact { laplace::integer_deallocate { .id = 42 } });
}

TEST("entity set") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .set_id(3)
              .set(2, 42) ==
          laplace::impact { laplace::integer_set {
              .id = 3, .index = 1, .value = 42 } });
}

TEST("entity get") {
  auto s      = laplace::state {};
  std::ignore = s.apply(laplace::impact {
      laplace::integer_allocate_into { .id = 0, .size = 1 } });
  std::ignore = s.apply(laplace::impact {
      laplace::integer_set { .id = 0, .index = 0, .value = 42 } });
  while (s.adjust()) { }

  REQUIRE(laplace::entity {}
              .setup({ { .id = 7 } })
              .set_access(laplace::access { s })
              .set_id(0)
              .get(7, -1) == 42);
}

TEST("entity add") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .set_id(3)
              .add(2, 42) ==
          laplace::impact { laplace::integer_add {
              .id = 3, .index = 1, .delta = 42 } });
}

TEST("entity random") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .set_id(3)
              .random(2, 1, 100) ==
          laplace::impact {
              laplace::integer_random { .min          = 1,
                                        .max          = 100,
                                        .return_id    = 3,
                                        .return_index = 1,
                                        .return_size  = 1 } });
}

TEST("entity spawn to") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 1 }, { .id = 2 } })
              .spawn_to({ laplace::entity {}
                              .setup({ { .id = 42 } })
                              .set_id(1)
                              .point(42) }) ==
          laplace::impact { laplace::integer_allocate {
              .size = 2, .return_id = 1, .return_index = 0 } });
}

TEST("entity error impacts") {
  auto const error = laplace::entity {}.setup(
      { { .id = 1 }, { .id = 1 } });
  REQUIRE(error.spawn() == laplace::impact { laplace::noop {} });
  REQUIRE(error.remove() == laplace::impact { laplace::noop {} });
  REQUIRE(error.set(1, 0) == laplace::impact { laplace::noop {} });
  REQUIRE(error.add(1, 0) == laplace::impact { laplace::noop {} });
  REQUIRE(error.random(1, 1, 100) ==
          laplace::impact { laplace::noop {} });
  REQUIRE(error.spawn_to({ .id = 0, .index = 0 }) ==
          laplace::impact { laplace::noop {} });
}

TEST("entity error access") {
  auto const error = laplace::entity {}.setup(
      { { .id = 1 }, { .id = 1 } });
  REQUIRE(error.get(1, -1) == -1);
}

TEST("entity setup with large id") {
  REQUIRE(laplace::entity {}
              .setup({ { .id = 0x1000000000000 } })
              .index_of(0x1000000000000) == 0);
  REQUIRE(laplace::entity {}
              .setup({ { .id = numeric_limits<ptrdiff_t>::max() } })
              .index_of(numeric_limits<ptrdiff_t>::max()) == 0);
}
