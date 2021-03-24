/*  laplace/platform/win32/win32_glcontext.cpp
 *
 *      Win32 OpenGL context implementation.
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
#include "../gldef.h"
#include "glcontext.h"
#include "window.h"
#include <algorithm>
#include <iostream>

extern "C" {

  /*  Require high performance for Nvidia.
   */
  __declspec(dllexport) uint32_t NvOptimusEnablement = 1;

  /*  Require high performance for AMD.
   */
  __declspec(dllexport) uint32_t
      AmdPowerXpressRequestHighPerformance = 1;
}

namespace laplace::win32 {
  using std::shared_ptr;

  bool glcontext::m_is_forward_compatible = false;

  glcontext::glcontext(shared_ptr<window> win) {
    if (!win) {
      error(__FUNCTION__, "No window.");
      return;
    }

    if (!gl_init()) {
      error(__FUNCTION__, "WGL initialization failed.");
      return;
    }

    m_window = win;

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof pfd);

    pfd.nSize    = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    if (m_is_forward_compatible) {
      /*  OpenGL forward-compatibility.
       */
      pfd.iLayerType = PFD_MAIN_PLANE;
    }

    m_hDC = GetDC(win->get_native_handle());

    if (!m_hDC) {
      error(__FUNCTION__, "GetDC failed.");
      cleanup();
      return;
    }

    int pf = ChoosePixelFormat(m_hDC, &pfd);

    if (!SetPixelFormat(m_hDC, pf, &pfd)) {
      error(__FUNCTION__, "SetPixelFormat failed.");
      cleanup();
      return;
    }

    m_hRC = wglCreateContext(m_hDC);

    if (!m_hRC) {
      error(__FUNCTION__, "wglCreateContext failed.");
      cleanup();
      return;
    }

    if (!wglMakeCurrent(m_hDC, m_hRC)) {
      error(__FUNCTION__, "wglMakeCurrent failed.");
      cleanup();
      return;
    }

    if (gl_preload() && gl::has("WGL_ARB_create_context")) {
      /*  Specify the OpenGL version.
       */

      int32_t major, minor;

      gl::glGetIntegerv(gl::GL_MAJOR_VERSION, &major);
      gl::glGetIntegerv(gl::GL_MINOR_VERSION, &minor);

      int32_t attrs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,
                          major,
                          WGL_CONTEXT_MINOR_VERSION_ARB,
                          minor,
                          WGL_CONTEXT_FLAGS_ARB,
                          0,
                          0 };

      if (m_is_forward_compatible) {
        attrs[5] = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
      }

      HGLRC hRC = wglCreateContextAttribsARB(
          m_hDC, nullptr, attrs);

      if (!hRC) {
        error(
            __FUNCTION__, "wglCreateContextAttribsARB failed.");
        cleanup();
        return;
      }

      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(m_hRC);

      m_hRC = hRC;

      if (!wglMakeCurrent(m_hDC, m_hRC)) {
        error(__FUNCTION__, "wglMakeCurrent failed.");
        cleanup();
        return;
      }
    }

    if (!gl::init()) {
      error(__FUNCTION__, "OpenGL initialization failed.");
      cleanup();
      return;
    }
  }

  glcontext::~glcontext() {
    cleanup();
  }

  void glcontext::swap_buffers() {
    SwapBuffers(m_hDC);
  }

  void glcontext::set_forward_compatible(bool is_forward_compatible) {
    m_is_forward_compatible = is_forward_compatible;
  }

  void glcontext::cleanup() {
    if (m_hDC) {
      if (m_hRC) {
        wglMakeCurrent(m_hDC, nullptr);
        wglDeleteContext(m_hRC);
      }

      if (m_window && m_window->get_native_handle()) {
        ReleaseDC(m_window->get_native_handle(), m_hDC);
      }
    }

    gl_cleanup();

    m_hDC = nullptr;
    m_hRC = nullptr;
  }
}
