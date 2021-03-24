/*  laplace/platform/thread.h
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

#ifndef laplace_platform_thread_h
#define laplace_platform_thread_h

#include <thread>

namespace laplace::platform {
  namespace priority {
    enum : int {
      idle = 1,
      lowest,
      low,
      normal,
      high,
      highest,
      critical
    };
  }
}

#endif
