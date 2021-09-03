/*  laplace/core/c_log.cpp
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

#include "log.h"

#include "string.h"
#include <cstdarg>
#include <iostream>
#include <mutex>
#include <shared_mutex>

namespace laplace {
  using std::unique_lock, std::shared_lock, std::mutex,
      std::shared_mutex, std::string_view, std::u8string_view,
      std::cerr;

#ifdef LAPLACE_VERBOSE
  static bool         g_verbose = true;
  static shared_mutex g_verbose_lock;

  void set_verbose(bool is_verbose) noexcept {
    auto _ul  = unique_lock(g_verbose_lock);
    g_verbose = is_verbose;
  }

  auto is_verbose() noexcept -> bool {
    auto _sl = shared_lock(g_verbose_lock);
    return g_verbose;
  }

  void verb(string_view s) noexcept {
    if (is_verbose()) {
      log(s);
    }
  }

  void verb(u8string_view s) noexcept {
    if (is_verbose()) {
      log(s);
    }
  }
#endif

  static auto g_log_lock = mutex {};

  void log(string_view s) noexcept {
    try {
      auto _ul = unique_lock(g_log_lock);
      cerr << s << '\n';
    } catch (...) { }
  }

  void log(u8string_view s) noexcept {
    log(as_ascii_string(s));
  }

  void error_(string_view message, string_view loc) noexcept {
    auto _ul = unique_lock(g_log_lock);
    if (loc.empty()) {
      cerr << "[ error ] " << message << '\n';
    } else {
      cerr << "[ error in " << loc << " ] " << message << '\n';
    }
  }

  void error_(u8string_view message, string_view loc) noexcept {
    error_(as_ascii_string(message), loc);
  }
}
