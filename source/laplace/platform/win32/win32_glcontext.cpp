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

#include "glcontext.h"

#include "../../core/defs.h"
#include "../gldef.h"
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
    if (!gl_load_library()) {
      get_global_log()(log_event::error,
                       "Unable to load OpenGL library.",
                       __FUNCTION__);
      return;
    }

    if (!win) {
      get_global_log()(log_event::error, "No window.", __FUNCTION__);
      return;
    }

    m_window = win;

    auto pfd = PIXELFORMATDESCRIPTOR {};

    pfd.nSize    = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    m_hDC = GetDC(win->get_native_handle());

    if (!m_hDC) {
      get_global_log()(log_event::error, "GetDC failed.",
                       __FUNCTION__);
      cleanup();
      return;
    }

    auto pf = ChoosePixelFormat(m_hDC, &pfd);

    if (!SetPixelFormat(m_hDC, pf, &pfd)) {
      get_global_log()(log_event::error, "SetPixelFormat failed.",
                       __FUNCTION__);
      cleanup();
      return;
    }

    m_hRC = wglCreateContext(m_hDC);

    if (!m_hRC) {
      get_global_log()(log_event::error, "wglCreateContext failed.",
                       __FUNCTION__);
      cleanup();
      return;
    }

    if (!wglMakeCurrent(m_hDC, m_hRC)) {
      get_global_log()(log_event::error, "wglMakeCurrent failed.",
                       __FUNCTION__);
      cleanup();
      return;
    }

    if (gl_preload_context() &&
        gl::has_extension("WGL_ARB_create_context")) {
      /*  Specify the OpenGL version.
       */

      auto major = int32_t {};
      auto minor = int32_t {};

      gl::glGetIntegerv(gl::GL_MAJOR_VERSION, &major);
      gl::glGetIntegerv(gl::GL_MINOR_VERSION, &minor);

      int32_t attrs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,
                          major,
                          WGL_CONTEXT_MINOR_VERSION_ARB,
                          minor,
                          WGL_CONTEXT_FLAGS_ARB,
                          0,
                          0 };

      if (m_is_forward_compatible)
        attrs[5] = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

      auto hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attrs);

      if (hRC == nullptr) {
        get_global_log()(log_event::error,
                         "wglCreateContextAttribsARB failed.",
                         __FUNCTION__);
        cleanup();
        return;
      }

      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(m_hRC);

      m_hRC = hRC;

      if (!wglMakeCurrent(m_hDC, m_hRC)) {
        get_global_log()(log_event::error, "wglMakeCurrent failed.",
                         __FUNCTION__);
        cleanup();
        return;
      }
    }

    if (!gl::load_functions(get_global_log())) {
      get_global_log()(log_event::error,
                       "OpenGL initialization failed.", __FUNCTION__);
      cleanup();
      return;
    }
  }

  glcontext::~glcontext() {
    cleanup();

    gl_free_library();
  }

  void glcontext::swap_buffers() {
    SwapBuffers(m_hDC);
  }

  void glcontext::set_forward_compatible(bool is_forward_compatible) {
    m_is_forward_compatible = is_forward_compatible;
  }

  void glcontext::cleanup() {
    if (m_hDC != nullptr) {
      if (m_hRC != nullptr) {
        wglMakeCurrent(m_hDC, nullptr);
        wglDeleteContext(m_hRC);
      }

      if (m_window && m_window->get_native_handle() != nullptr)
        ReleaseDC(m_window->get_native_handle(), m_hDC);
    }

    m_hDC = nullptr;
    m_hRC = nullptr;
  }
}
