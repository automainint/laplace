#include "../../laplace/entity.h"
#include "../../laplace/impact.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  using std::numeric_limits;

  TEST_CASE("create entity") {
    REQUIRE(!entity {}.error());
  }

  TEST_CASE("entity size is zero by default") {
    REQUIRE(entity {}.size() == 0);
  }

  TEST_CASE("entity default id") {
    REQUIRE(entity {}.id() == id_undefined);
  }

  TEST_CASE("entity default access") {
    REQUIRE(entity {}.access() == access {});
  }

  TEST_CASE("entity set id") {
    REQUIRE(entity {}.set_id(42).id() == 42);
  }

  TEST_CASE("entity set access") {
    auto s = state {};
    REQUIRE(entity {}.set_access(access { s }).access() ==
            access { s });
  }

  TEST_CASE("entity setup fields") {
    REQUIRE(entity {}.setup({}).size() == 0);
  }

  TEST_CASE("entity setup 3 fields") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 }, { .id = 3 } })
                .size() == 3);
  }

  TEST_CASE("entity setup twice") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .setup({ { .id = 3 }, { .id = 4 }, { .id = 5 } })
                .size() == 5);
  }

  TEST_CASE("entity setup and get index") {
    REQUIRE(entity {}.setup({ { .id = 42 } }).index_of(42) == 0);
  }

  TEST_CASE("entity setup 2 fields and get index") {
    REQUIRE(
        entity {}.setup({ { .id = 1 }, { .id = 42 } }).index_of(42) ==
        1);
  }

  TEST_CASE("entity get index may fail") {
    REQUIRE(entity {}.index_of(42) == index_undefined);
  }

  TEST_CASE("entity setup unsorted") {
    REQUIRE(entity {}
                .setup({ { .id = 3 }, { .id = 2 }, { .id = 1 } })
                .index_of(1) == 0);
  }

  TEST_CASE("entity setup repeating will fail") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 }, { .id = 1 } })
                .error());
  }

  TEST_CASE("entity negative field ids not allowed") {
    REQUIRE(entity {}.setup({ { .id = -3 } }).error());
  }

  TEST_CASE("entity setup twice with repeating") {
    auto e = entity {}
                 .setup({ { .id = 1 }, { .id = 2 } })
                 .setup({ { .id = 1 }, { .id = 3 }, { .id = 4 } });
    REQUIRE(!e.error());
    REQUIRE(e.size() == 4);
    REQUIRE(e.index_of(1) == 0);
    REQUIRE(e.index_of(2) == 1);
    REQUIRE(e.index_of(3) == 2);
    REQUIRE(e.index_of(4) == 3);
  }

  TEST_CASE("entity propagate error") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .setup({ { .id = 42 } })
                .error());
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .set_id(0)
                .error());
  }

  TEST_CASE("entity spawn with id") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .set_id(42)
                .spawn() ==
            impact { integer_allocate_into { .id = 42, .size = 2 } });
  }

  TEST_CASE("entity remove") {
    REQUIRE(entity {}.set_id(42).remove() ==
            impact { integer_deallocate { .id = 42 } });
  }

  TEST_CASE("entity set") {
    REQUIRE(
        entity {}
            .setup({ { .id = 1 }, { .id = 2 } })
            .set_id(3)
            .set(2, 42) ==
        impact { integer_set { .id = 3, .index = 1, .value = 42 } });
  }

  TEST_CASE("entity get") {
    auto s      = state {};
    std::ignore = s.apply(
        integer_allocate_into { .id = 0, .size = 1 });
    std::ignore = s.apply(
        integer_set({ .id = 0, .index = 0, .value = 42 }));
    while (s.adjust()) { }

    REQUIRE(entity {}
                .setup({ { .id = 7 } })
                .set_access(access { s })
                .set_id(0)
                .get(7, -1) == 42);
  }

  TEST_CASE("entity add") {
    REQUIRE(
        entity {}
            .setup({ { .id = 1 }, { .id = 2 } })
            .set_id(3)
            .add(2, 42) ==
        impact { integer_add { .id = 3, .index = 1, .delta = 42 } });
  }

  TEST_CASE("entity random") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .set_id(3)
                .random(2, 1, 100) ==
            impact { integer_random { .min          = 1,
                                      .max          = 100,
                                      .return_id    = 3,
                                      .return_index = 1,
                                      .return_size  = 1 } });
  }

  TEST_CASE("entity spawn to") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .spawn_to({ entity {}
                                .setup({ { .id = 42 } })
                                .set_id(1)
                                .point(42) }) ==
            impact { integer_allocate {
                .size = 2, .return_id = 1, .return_index = 0 } });
  }

  TEST_CASE("entity error impacts") {
    auto const error = entity {}.setup({ { .id = 1 }, { .id = 1 } });
    REQUIRE(error.spawn() == impact { noop {} });
    REQUIRE(error.remove() == impact { noop {} });
    REQUIRE(error.set(1, 0) == impact { noop {} });
    REQUIRE(error.add(1, 0) == impact { noop {} });
    REQUIRE(error.random(1, 1, 100) == impact { noop {} });
    REQUIRE(error.spawn_to({ .id = 0, .index = 0 }) ==
            impact { noop {} });
  }

  TEST_CASE("entity error access") {
    auto const error = entity {}.setup({ { .id = 1 }, { .id = 1 } });
    REQUIRE(error.get(1, -1) == -1);
  }

  TEST_CASE("entity setup with large id") {
    REQUIRE(entity {}
                .setup({ { .id = 0x1000000000000 } })
                .index_of(0x1000000000000) == 0);
    REQUIRE(entity {}
                .setup({ { .id = numeric_limits<ptrdiff_t>::max() } })
                .index_of(numeric_limits<ptrdiff_t>::max()) == 0);
  }

  /*TEST_CASE("entity index of in constexpr context") {
    constexpr auto n = entity {}.setup({ { .id = 42 } }).index_of(42);
    REQUIRE(n == 0);
  }*/
}
