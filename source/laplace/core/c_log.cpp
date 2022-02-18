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

#include <ctime>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace laplace {
  using std::unique_lock, std::shared_lock, std::shared_mutex,
      std::string_view, std::u8string_view, std::cout, std::cerr,
      std::flush, std::ostringstream;

  static auto g_log_lock = shared_mutex {};
  static bool g_verbose  = true;

  static auto g_log_default = [](log_event event, string_view message,
                                 string_view origin) noexcept {
    try {
      auto _ul = unique_lock(g_log_lock);
      if (event == log_event::verbose && !g_verbose)
        return;
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
      if (event == log_event::message || event == log_event::verbose)
        cout << ss.str() << flush;
      else
        cerr << ss.str() << flush;
    } catch (std::exception &error) {
      std::cerr << "[ Log Error ] " << error.what() << '\n'
                << std::flush;
    } catch (...) {
      std::cerr << "[ Log Error ] Unknown exception!" << std::flush;
    }
  };

  static auto g_log = log_handler { g_log_default };

  void disable_log() noexcept {
    setup_global_log([](log_event, string_view, string_view) {});
  }

  void setup_global_log(log_handler const &write) noexcept {
    if constexpr (_log_enabled) {
      auto _ul = unique_lock(g_log_lock);
      g_log    = write ? write : g_log_default;
    }
  }

  auto get_global_log() noexcept -> log_handler {
    return g_log;
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
}
