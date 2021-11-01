/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_platform_linux_glx_h
#define laplace_platform_linux_glx_h

#include "../opengl.h"
#include "xlib.h"

namespace laplace::linux {
  auto gl_load_library() noexcept -> bool;

  void gl_free_library() noexcept;

  [[nodiscard]] auto gl_get_proc_address(char const *name) noexcept
      -> gl::ptr_function;

  auto gl_is_loaded() noexcept -> bool;

  enum glx_enum : uint32_t {
    GLX_RGBA         = 4,
    GLX_DOUBLEBUFFER = 5,
    GLX_DEPTH_SIZE   = 12,
  };

  using GLXDrawable = Drawable;
  using GLXContext  = struct GLXContext_tag *;

  using pfn_glXChooseVisual = XVisualInfo *(*) (Display *dpy,
                                                int      screen,
                                                int     *attribList);

  using pfn_glXGetProcAddress =
      laplace::gl::ptr_function (*)(char const *name);

  using pfn_glXSwapBuffers = void (*)(Display    *dpy,
                                      GLXDrawable drawable);

  using pfn_glXCreateContext = GLXContext (*)(Display     *dpy,
                                              XVisualInfo *vis,
                                              GLXContext   shareList,
                                              int          direct);

  using pfn_glXMakeCurrent = int (*)(Display    *dpy,
                                     GLXDrawable drawable,
                                     GLXContext  ctx);

  using pfn_glXDestroyContext = void (*)(Display   *dpy,
                                         GLXContext ctx);

  extern pfn_glXGetProcAddress glXGetProcAddress;
  extern pfn_glXChooseVisual   glXChooseVisual;
  extern pfn_glXSwapBuffers    glXSwapBuffers;
  extern pfn_glXCreateContext  glXCreateContext;
  extern pfn_glXMakeCurrent    glXMakeCurrent;
  extern pfn_glXDestroyContext glXDestroyContext;
}

#endif
