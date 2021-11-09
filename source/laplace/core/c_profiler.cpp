/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "profiler.h"

#include <chrono>
#include <mutex>
#include <string>

namespace laplace::core::profiler {
  using std::mutex, std::unique_lock, std::string, std::string_view,
      std::chrono::microseconds, std::chrono::steady_clock,
      std::chrono::duration_cast;

  static auto g_lock = mutex {};
  static auto g_fn   = fn_segment {};
  static auto g_name = string {};
  static auto g_time = steady_clock::time_point {};

  void setup(fn_segment fn) noexcept {
    auto _ul = unique_lock(g_lock);
    g_fn     = fn;
  }

  void fence(string_view name) noexcept {
    auto       _ul  = unique_lock(g_lock);
    auto const time = steady_clock::now();
    if (g_fn && !g_name.empty() && !name.empty()) {
      g_fn(g_name, name,
           duration_cast<microseconds>(time - g_time).count());
    }
    g_name = name;
    g_time = time;
  }
}
