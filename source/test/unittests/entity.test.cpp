/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/entity.h"
#include <catch2/catch.hpp>

namespace laplace::test {
  TEST_CASE("Create entity") {
    REQUIRE(!entity {}.is_error());
  }

  TEST_CASE("Entity size is zero by default") {
    REQUIRE(entity {}.get_size() == 0);
  }

  TEST_CASE("Entity default id") {
    REQUIRE(entity {}.get_id() == id_undefined);
  }

  TEST_CASE("Entity set id") {
    REQUIRE(entity {}.set_id(42).get_id() == 42);
  }

  TEST_CASE("Entity setup fields") {
    REQUIRE(entity {}.setup({}).get_size() == 0);
  }

  TEST_CASE("Entity setup 3 fields") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 }, { .id = 3 } })
                .get_size() == 3);
  }

  TEST_CASE("Entity setup twice") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 } })
                .setup({ { .id = 3 }, { .id = 4 }, { .id = 5 } })
                .get_size() == 5);
  }

  TEST_CASE("Entity setup and get index") {
    REQUIRE(entity {}.setup({ { .id = 42 } }).index_of(42) == 0);
  }

  TEST_CASE("Entity setup 2 fields and get index") {
    REQUIRE(
        entity {}.setup({ { .id = 1 }, { .id = 42 } }).index_of(42) ==
        1);
  }

  TEST_CASE("Entity get index may fail") {
    REQUIRE(entity {}.index_of(42) == index_undefined);
  }

  TEST_CASE("Entity setup unsorted") {
    REQUIRE(entity {}
                .setup({ { .id = 3 }, { .id = 2 }, { .id = 1 } })
                .index_of(1) == 0);
  }

  TEST_CASE("Entity setup repeating will fail") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 2 }, { .id = 1 } })
                .is_error());
  }

  TEST_CASE("Entity setup twice with repeating") {
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

  TEST_CASE("Entity propagate error") {
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .setup({ { .id = 42 } })
                .is_error());
    REQUIRE(entity {}
                .setup({ { .id = 1 }, { .id = 1 } })
                .set_id(0)
                .is_error());
  }

  /*TEST_CASE("Entity constexpr index of") {
    constexpr auto index =
        entity {}.setup({ { .id = 42 } }).index_of(42);
    REQUIRE(index == 0);
  }*/
}
