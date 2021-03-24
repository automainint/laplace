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

#if !defined(LAPLACE_POSIX_SOCKETS) && \
    (defined(_WIN32) || defined(_WINDOWS))
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#endif

#if !defined(LAPLACE_PLATFORM_DUMMY) && \
    (defined(_WIN32) || defined(_WINDOWS))

#  include "win32/audio.h"
#  include "win32/clipboard.h"
#  include "win32/glcontext.h"
#  include "win32/input.h"
#  include "win32/thread.h"
#  include "win32/window.h"

namespace laplace::platform {
  using namespace win32;
}
#else
#  include "dummy.h"

namespace laplace::platform {
  using namespace dummy;
}
#endif

namespace laplace::platform {
  using ptr_window    = std::shared_ptr<window>;
  using ptr_input     = std::shared_ptr<input>;
  using ptr_glcontext = std::shared_ptr<glcontext>;
  using ptr_audio     = std::shared_ptr<audio>;
  using ptr_clipboard = std::shared_ptr<clipboard>;

  using ref_window    = window &;
  using ref_input     = input &;
  using ref_glcontext = glcontext &;
  using ref_audio     = audio &;
  using ref_clipboard = clipboard &;
}

#endif
