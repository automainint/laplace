/*  laplace/platform/wrap.h
 *
 *      Platform API.
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

#ifndef laplace_platform_wrap_h
#define laplace_platform_wrap_h

#include "../core/defs.h"

#define LAPLACE_USING_PLATFORM(PREFIX_)                             \
  using PREFIX_::gl_get_proc_address, PREFIX_::set_background_mode, \
      PREFIX_::set_realtime_mode, PREFIX_::set_thread_priority,     \
      PREFIX_::input, PREFIX_::window, PREFIX_::glcontext,          \
      PREFIX_::audio, PREFIX_::clipboard

#if !defined(LAPLACE_PLATFORM_DUMMY) && \
    (defined(_WIN32) || defined(_WINDOWS))
#  include "win32/audio.h"
#  include "win32/clipboard.h"
#  include "win32/glcontext.h"
#  include "win32/input.h"
#  include "win32/thread.h"
#  include "win32/window.h"

namespace laplace::platform {
  LAPLACE_USING_PLATFORM(win32);
}
#elif !defined(LAPLACE_PLATFORM_DUMMY) && defined(__linux__)
#  include "linux/audio.h"
#  include "linux/clipboard.h"
#  include "linux/glcontext.h"
#  include "linux/input.h"
#  include "linux/thread.h"
#  include "linux/window.h"

namespace laplace::platform {
  LAPLACE_USING_PLATFORM(linux);
}
#else
#  include "dummy.h"

namespace laplace::platform {
  LAPLACE_USING_PLATFORM(dummy);
}
#endif

#endif
