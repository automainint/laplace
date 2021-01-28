/*  Platform API.
 */

#pragma once

#if defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>

#include "win32/window.h"
#include "win32/input.h"
#include "win32/glcontext.h"
#include "win32/audio.h"
#include "win32/clipboard.h"
#include "win32/thread.h"

namespace laplace::platform
{
    using namespace win32;
}
#else
#include "dummy.h"

namespace laplace::platform
{
    using namespace dummy;
}
#endif

namespace laplace::platform
{
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
