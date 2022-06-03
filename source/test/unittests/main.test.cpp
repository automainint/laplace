/*  Copyright (c) 2022 Mitya Selivanov
 */

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

auto main(int argc, char **argv) -> int {
  return Catch::Session().run(argc, argv);
}
