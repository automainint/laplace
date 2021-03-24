/*  laplace/platform/win32/glcontext.h
 *
 *      Win32 OpenGL context declarations.
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

#ifndef laplace_platform_win32_glcontext_h
#define laplace_platform_win32_glcontext_h

#include "../events.h"
#include "wgl.h"
#include "win.predef.h"
#include <memory>

namespace laplace::win32 {
  class window;

  class glcontext {
  public:
    glcontext(std::shared_ptr<window> win);
    ~glcontext();

    void swap_buffers();

    static void set_forward_compatible(bool is_forward_compatible);

  private:
    void cleanup();

    static bool m_is_forward_compatible;

    std::shared_ptr<window> m_window;

    HDC   m_hDC = nullptr;
    HGLRC m_hRC = nullptr;
  };
}

#endif
