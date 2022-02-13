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
#include <ctime>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace laplace {
  using std::unique_lock, std::shared_lock, std::shared_mutex,
      std::string_view, std::u8string_view, std::cout, std::cerr,
      std::flush, std::ostringstream;

  static auto g_log_default = [](log_event event, string_view message,
                                 string_view origin) {
    try {
      auto ss = ostringstream {};
      if (origin.empty())
        switch (event) {
          case log_event::warning: ss << "[ Warning ] "; break;
          case log_event::error: ss << "[ Error ] "; break;
          case log_event::fatal: ss << "[ Fatal Error ] "; break;
          default:;
        }
      else
        switch (event) {
          case log_event::warning:
            ss << "[ Warning from " << origin << " ] ";
            break;
          case log_event::error:
            ss << "[ Error in " << origin << " ] ";
            break;
          case log_event::fatal:
            ss << "[ Fatal Error in " << origin << " ] ";
            break;
          default:;
        }
      ss << message;
      if (ss.str().size() < 70)
        for (auto i = ss.str().size(); i < 70; i++) ss << ' ';
      else
        ss << '\n';
      char buf[40] = {};
      auto t       = std::time(nullptr);
      if (std::strftime(buf, sizeof buf, "%T", std::localtime(&t)) >
          0)
        ss << buf;
      ss << '\n';
      if (event == log_event::message)
        cout << ss.str() << flush;
      else
        cerr << ss.str() << flush;
    } catch (...) { }
  };

  static bool g_verbose  = true;
  static auto g_log_lock = shared_mutex {};
  static auto g_log      = fn_log { g_log_default };

  void disable_log() noexcept {
    setup_log([](log_event, string_view, string_view) {});
  }

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

  void log(log_event event, string_view message,
           string_view origin) noexcept {
    if constexpr (_log_enabled) {
      auto _ul = unique_lock(g_log_lock);
      g_log(event, message, origin);
    }
  }

  void log(log_event event, u8string_view message,
           u8string_view origin) noexcept {
    if constexpr (_log_enabled) {
      log(event, as_ascii_string(message), as_ascii_string(origin));
    }
  }

  void verb(string_view message, string_view origin) noexcept {
    if constexpr (_log_enabled) {
      if (is_verbose())
        log(log_event::message, message, origin);
    }
  }

  void verb(u8string_view message, u8string_view origin) noexcept {
    if constexpr (_log_enabled) {
      if (is_verbose())
        log(log_event::message, message, origin);
    }
  }

  void error_(string_view message, string_view origin) noexcept {
    if constexpr (_log_enabled) {
      log(log_event::error, message, origin);
    }
  }

  void error_(u8string_view message, u8string_view origin) noexcept {
    if constexpr (_log_enabled) {
      log(log_event::error, message, origin);
    }
  }
}
