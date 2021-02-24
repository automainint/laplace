/*  laplace/platform/win32/win32_thread.cpp
 *
 *      Win32 concurrency system utilities.
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

#define WIN32_LEAN_AND_MEAN
#define laplace_windows_header
#include <windows.h>

#include "../../core/defs.h"
#include "thread.h"
#include <algorithm>

#undef min
#undef max

namespace laplace::win32 {
  using std::min, std::max, std::thread;

  static void set_thread_priority(HANDLE th, int priority) {
    constexpr int ids[] = {
      THREAD_PRIORITY_IDLE,         THREAD_PRIORITY_LOWEST,
      THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
      THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST,
      THREAD_PRIORITY_TIME_CRITICAL
    };

    auto n = max(1, min(priority, 7)) - 1;

    SetThreadPriority(th, ids[n]);
  }

  void win32::set_background_mode(bool is_background_mode) {
    static bool in_background = false;

    if (in_background != is_background_mode) {
      auto status = SetPriorityClass(
          GetCurrentProcess(),
          is_background_mode ? PROCESS_MODE_BACKGROUND_BEGIN
                             : PROCESS_MODE_BACKGROUND_END);

      if (!status) {
        /*  For Windows XP.
         */

        SetThreadPriority(GetCurrentThread(),
                          is_background_mode
                              ? THREAD_MODE_BACKGROUND_BEGIN
                              : THREAD_MODE_BACKGROUND_END);
      }

      in_background = is_background_mode;
    }
  }

  void win32::set_realtime_mode(bool is_realtime_mode) {
    SetPriorityClass(
        GetCurrentProcess(), is_realtime_mode
                                 ? REALTIME_PRIORITY_CLASS
                                 : ABOVE_NORMAL_PRIORITY_CLASS);
  }

  void win32::set_thread_priority(int priority) {
    set_thread_priority(GetCurrentThread(), priority);
  }

  void win32::set_thread_priority(thread &th, int priority) {
    set_thread_priority(th.native_handle(), priority);
  }
}
