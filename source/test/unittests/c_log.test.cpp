/*  Copyright (c) 2022 Mitya Selivanov
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
  using std::string_view, std::string, std::u8string_view,
      std::u8string;

  TEST(core, log_verbose) {
    auto log = get_global_log();
    log(log_event::verbose, "Log verbose test.", "Test");
  }

  TEST(core, log_message) {
    auto log = get_global_log();
    log(log_event::message, "Log message test.", "Test");
  }

  TEST(core, log_warning) {
    auto log = get_global_log();
    log(log_event::warning, "Log warning test.", "Test");
  }

  TEST(core, log_error) {
    auto log = get_global_log();
    log(log_event::error, "Log error test.", "Test");
  }

  TEST(core, log_fatal_error) {
    auto log = get_global_log();
    log(log_event::fatal, "Log fatal error test.", "Test");
  }

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
      auto       a = string {};
      auto const b = string { "ascii test" };

      setup_global_log([&](log_event, string_view text, string_view) {
        a = text;
      });

      auto log = get_global_log();
      log(log_event::message, b, "");
      EXPECT_EQ(a, b);

      setup_global_log({});
    }
  }
}
