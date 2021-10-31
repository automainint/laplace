/*  laplace/platform/linux/thread.h
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

#ifndef laplace_platform_linux_thread_h
#define laplace_platform_linux_thread_h

#include <thread>
#include "xlib.h"

namespace laplace::linux {
  void set_background_mode(bool is_background_mode) noexcept;
  void set_realtime_mode(bool is_realtime_mode) noexcept;
  void set_thread_priority(int priority) noexcept;
  void set_thread_priority(std::thread &th, int priority) noexcept;
}

#endif
