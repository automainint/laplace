/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/entity.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("create entity") {
    REQUIRE(!entity {}.is_error());
  }

  TEST_CASE("entity size is zero by default") {
    REQUIRE(entity {}.get_size() == 0);
  }

  TEST_CASE("entity default id") {
    REQUIRE(entity {}.get_id() == id_undefined);
  }

  TEST_CASE("entity default access") {
    REQUIRE(entity {}.get_access() == access {});
  }

  TEST_CASE("entity set id") {
    REQUIRE(entity {}.set_id(42).get_id() == 42);
  }

  TEST_CASE("entity set access") {
    auto s = state {};
    REQUIRE(entity {}.set_access(access { s }).get_access() ==
            access { s });
  }

  TEST_CASE("entity setup fields") {
    REQUIRE(entity {}.setup({}).get_size() == 0);
  }

  TEST_CASE("entity setup 3 fields") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 }, { .id = 3 } })
                .get_size() == 3);
  }

  TEST_CASE("entity setup twice") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .setup({ { .id = 3 }, { .id = 4 }, { .id = 5 } })
                .get_size() == 5);
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
                .is_error());
  }

  TEST_CASE("entity setup twice with repeating") {
    auto e = entity {}
                 .setup({ { .id = 1 }, { .id = 2 } })
                 .setup({ { .id = 1 }, { .id = 3 }, { .id = 4 } });
    REQUIRE(!e.is_error());
    REQUIRE(e.get_size() == 4);
    REQUIRE(e.index_of(1) == 0);
    REQUIRE(e.index_of(2) == 1);
    REQUIRE(e.index_of(3) == 2);
    REQUIRE(e.index_of(4) == 3);
  }

  TEST_CASE("entity propagate error") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .setup({ { .id = 42 } })
                .is_error());
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .set_id(0)
                .is_error());
  }

  TEST_CASE("entity spawn without id") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .spawn({ .return_id = 3, .return_index = 42 }) ==
            impact { integer_allocate {
                .size = 2, .return_id = 3, .return_index = 42 } });
  }

  TEST_CASE("entity spawn with id") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .set_id(42)
                .spawn() ==
            impact { integer_reallocate { .id = 42, .size = 2 } });
  }

  TEST_CASE("entity remove") {
    REQUIRE(entity {}.set_id(42).remove() ==
            impact { integer_deallocate { .id = 42 } });
  }

  TEST_CASE("entity set by id") {
    REQUIRE(
        entity {}
            .setup({ { .id = 1 }, { .id = 2 } })
            .set_id(3)
            .set(2, 42) ==
        impact { integer_set { .id = 3, .index = 1, .value = 42 } });
  }

  TEST_CASE("entity set by index") {
    REQUIRE(
        entity {}
            .setup({ { .id = 1 }, { .id = 2 } })
            .set_id(3)
            .set_by_index(1, 42) ==
        impact { integer_set { .id = 3, .index = 1, .value = 42 } });
  }

  TEST_CASE("entity get by id") {
    auto s      = state {};
    std::ignore = s.apply(integer_reallocate { .id = 0, .size = 1 });
    std::ignore = s.apply(
        integer_set({ .id = 0, .index = 0, .value = 42 }));
    while (s.adjust()) { }

    REQUIRE(entity {}
                .setup({ { .id = 7 } })
                .set_access(access { s })
                .set_id(0)
                .get(7, -1) == 42);
  }

  TEST_CASE("entity get by index") {
    auto s      = state {};
    std::ignore = s.apply(integer_reallocate { .id = 0, .size = 1 });
    std::ignore = s.apply(
        integer_set({ .id = 0, .index = 0, .value = 42 }));
    while (s.adjust()) { }

    REQUIRE(entity {}
                .setup({ { .id = 7 } })
                .set_access(access { s })
                .set_id(0)
                .get_by_index(0, -1) == 42);
  }

  TEST_CASE("entity error impacts") {
    auto const error = entity {}.setup({ { .id = 1 }, { .id = 1 } });
    REQUIRE(error.spawn({ .return_id = 0, .return_index = 0 }) ==
            impact { noop {} });
    REQUIRE(error.spawn() == impact { noop {} });
    REQUIRE(error.remove() == impact { noop {} });
    REQUIRE(error.set(0, 0) == impact { noop {} });
    REQUIRE(error.set_by_index(0, 0) == impact { noop {} });
  }

  TEST_CASE("entity error access") {
    auto const error = entity {}.setup({ { .id = 1 }, { .id = 1 } });
    REQUIRE(error.get(1, -1) == -1);
    REQUIRE(error.get_by_index(0, -1) == -1);
  }

  /*TEST_CASE("entity constexpr index of") {
    constexpr auto index =
        entity {}.setup({ { .id = 42 } }).index_of(42);
    REQUIRE(index == 0);
  }*/
}
