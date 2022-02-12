/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "log.h"

#include "string.h"
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace laplace {
  using std::unique_lock, std::shared_lock, std::shared_mutex,
      std::string_view, std::u8string_view, std::cerr, std::flush,
      std::ostringstream;

  static auto g_log_default = [](string_view message) {
    cerr << message << '\n' << flush;
  };

  static bool g_verbose  = true;
  static auto g_log_lock = shared_mutex {};
  static auto g_log      = fn_log { g_log_default };

  void setup_log(fn_log const &write) noexcept {
    if constexpr (_log_enabled) {
      auto _ul = unique_lock(g_log_lock);
      g_log    = write ? write : g_log_default;
    }
  }

  void set_verbose(bool is_verbose) noexcept {
    if constexpr (_log_enabled) {
      auto _ul  = unique_lock(g_log_lock);
      g_verbose = is_verbose;
    }
  }

  auto is_verbose() noexcept -> bool {
    if constexpr (_log_enabled) {
      auto _sl = shared_lock(g_log_lock);
      return g_verbose;
    } else {
      return false;
    }
  }

  void log(string_view message) noexcept {
    if constexpr (_log_enabled) {
      auto _ul = unique_lock(g_log_lock);
      g_log(message);
    }
  }

  void log(u8string_view message) noexcept {
    if constexpr (_log_enabled) {
      log(as_ascii_string(message));
    }
  }

  void verb(string_view message) noexcept {
    if constexpr (_log_enabled) {
      if (is_verbose())
        log(message);
    }
  }

  void verb(u8string_view message) noexcept {
    if constexpr (_log_enabled) {
      if (is_verbose())
        log(message);
    }
  }

  void error_(string_view message, string_view loc) noexcept {
    if constexpr (_log_enabled) {
      auto ss = ostringstream {};
      ss << "[ error";
      if (!loc.empty())
        ss << " in " << loc;
      ss << " ] " << message << '\n';
      log(ss.str());
    }
  }

  void error_(u8string_view message, string_view loc) noexcept {
    if constexpr (_log_enabled) {
      error_(as_ascii_string(message), loc);
    }
  }
}
