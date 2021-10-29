/*  test/unittests/c_log.test.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/log.h"
#include "../../laplace/core/string.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, log_is_verbose) {
    if constexpr (_log_enabled) {
      auto const f = is_verbose();
      set_verbose(true);
      EXPECT_TRUE(is_verbose());
      set_verbose(false);
      EXPECT_FALSE(is_verbose());
      set_verbose(f);
    }
  }

  TEST(core, log_setup) {
    if constexpr (_log_enabled) {
      auto const f = is_verbose();

      auto       a = std::string {};
      auto const b = std::string { "ascii test" };
      auto const c = std::u8string { u8"utf-8 test" };

      setup_log([&](std::string_view text) { a = text; });

      log(b);
      EXPECT_EQ(a, b);
      log(c);
      EXPECT_EQ(a, as_ascii_string(c));

      setup_log({});
      set_verbose(f);
    }
  }

  TEST(core, log_set_verbose) {
    if constexpr (_log_enabled) {
      auto const f = is_verbose();

      auto       a = std::string {};
      auto const b = std::string { "ascii test" };
      auto const c = std::u8string { u8"utf-8 test" };

      setup_log([&](std::string_view text) { a = text; });

      set_verbose(false);
      verb(b);
      EXPECT_TRUE(a.empty());
      verb(c);
      EXPECT_TRUE(a.empty());

      set_verbose(true);
      verb(b);
      EXPECT_EQ(a, b);
      verb(c);
      EXPECT_EQ(a, as_ascii_string(c));

      setup_log({});
      set_verbose(f);
    }
  }
}
