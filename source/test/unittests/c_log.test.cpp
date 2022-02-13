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

  TEST(core, log_message) {
    log(log_event::message, "ASCII log test.", "Test");
    log(log_event::message, u8"UTF-8 log test.", u8"Test");
  }

  TEST(core, log_warning) {
    log(log_event::warning, "ASCII warning test.", "Test");
    log(log_event::warning, u8"UTF-8 warning test.", u8"Test");
  }

  TEST(core, log_error) {
    error_("ASCII error log test.", "Test");
    error_(u8"UTF-8 error log test.", u8"Test");
  }

  TEST(core, log_fatal_error) {
    log(log_event::fatal, "ASCII fatal error log test.", "Test");
    log(log_event::fatal, u8"UTF-8 fatal error log test.", u8"Test");
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
      auto const f = is_verbose();

      auto       a = string {};
      auto const b = string { "ascii test" };
      auto const c = u8string { u8"utf-8 test" };

      setup_log([&](log_event, string_view text, string_view) {
        a = text;
      });

      log(log_event::message, b, "");
      EXPECT_EQ(a, b);
      log(log_event::message, c, u8"");
      EXPECT_EQ(a, as_ascii_string(c));

      setup_log({});
      set_verbose(f);
    }
  }

  TEST(core, log_set_verbose) {
    if constexpr (_log_enabled) {
      auto const f = is_verbose();

      auto       a = string {};
      auto const b = string { "ascii test" };
      auto const c = u8string { u8"utf-8 test" };

      setup_log([&](log_event, string_view text, string_view) {
        a = text;
      });

      set_verbose(false);
      verb(b, "");
      EXPECT_TRUE(a.empty());
      verb(c, u8"");
      EXPECT_TRUE(a.empty());

      set_verbose(true);
      verb(b, "");
      EXPECT_EQ(a, b);
      verb(c, u8"");
      EXPECT_EQ(a, as_ascii_string(c));

      setup_log({});
      set_verbose(f);
    }
  }
}
