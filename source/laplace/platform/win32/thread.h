/*  laplace/platform/win32/thread.h
 *
 *      Win32 concurrency declarations.
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

#ifndef laplace_platform_win32_thread_h
#define laplace_platform_win32_thread_h

#include "../thread.h"

namespace laplace::win32 {
  void set_background_mode(bool is_background_mode);
  void set_realtime_mode(bool is_realtime_mode);

  /*  Thread priority from 1 to 7.
   */
  void set_thread_priority(int priority);
  void set_thread_priority(std::thread &th, int priority);
}

#endif
