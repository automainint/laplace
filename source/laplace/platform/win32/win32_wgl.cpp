/*  laplace/platform/win32/win32_wgl.cpp
 *
 *      WGL functions loading from OpenGL DLL.
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
#define __laplace__windows_header__
#include <windows.h>

#undef min
#undef max

#include "../../core/utils.h"
#include "../gldef.h"
#include "wgl.h"
#include <iostream>

#define LOAD(a)                                               \
  if (gl::a = reinterpret_cast<pfn_##a>(                      \
          GetProcAddress(opengl_dll, #a));                    \
      !gl::a) {                                               \
    error(__FUNCTION__, "Unable to get %s function from %s.", \
        #a, to_string(opengl_path).c_str());                  \
    status = false;                                           \
  }

namespace laplace::gl {
  extern std::vector<std::string> extensions;

  HMODULE opengl_dll = nullptr;

  pfn_wglCopyContext            wglCopyContext        = nullptr;
  pfn_wglCreateContext          wglCreateContext      = nullptr;
  pfn_wglCreateLayerContext     wglCreateLayerContext = nullptr;
  pfn_wglDeleteContext          wglDeleteContext      = nullptr;
  pfn_wglGetCurrentContext      wglGetCurrentContext  = nullptr;
  pfn_wglGetCurrentDC           wglGetCurrentDC       = nullptr;
  pfn_wglGetProcAddress         wglGetProcAddress     = nullptr;
  pfn_wglMakeCurrent            wglMakeCurrent        = nullptr;
  pfn_wglShareLists             wglShareLists         = nullptr;
  pfn_wglUseFontBitmapsA        wglUseFontBitmapsA    = nullptr;
  pfn_wglUseFontBitmapsW        wglUseFontBitmapsW    = nullptr;
  pfn_wglUseFontOutlinesA       wglUseFontOutlinesA   = nullptr;
  pfn_wglUseFontOutlinesW       wglUseFontOutlinesW   = nullptr;
  pfn_wglDescribeLayerPlane     wglDescribeLayerPlane = nullptr;
  pfn_wglSetLayerPaletteEntries wglSetLayerPaletteEntries =
      nullptr;
  pfn_wglGetLayerPaletteEntries wglGetLayerPaletteEntries =
      nullptr;
  pfn_wglRealizeLayerPalette wglRealizeLayerPalette = nullptr;
  pfn_wglSwapLayerBuffers    wglSwapLayerBuffers    = nullptr;
  pfn_wglCreateContextAttribsARB wglCreateContextAttribsARB =
      nullptr;
}

using namespace laplace;
using namespace std;

auto gl::platform_init() -> bool {
  if (opengl_dll) {
    /*  Already done.
     */
    return true;
  }

  if (opengl_dll = LoadLibraryW(opengl_path); !opengl_dll) {
    error(__FUNCTION__,
        "Unable to load OpenGL dynamic-link library from %s.",
        to_string(opengl_path).c_str());
    return false;
  }

  bool status = true;

  LOAD(wglCopyContext);
  LOAD(wglCreateContext);
  LOAD(wglCreateLayerContext);
  LOAD(wglDeleteContext);
  LOAD(wglGetCurrentContext);
  LOAD(wglGetCurrentDC);
  LOAD(wglGetProcAddress);
  LOAD(wglMakeCurrent);
  LOAD(wglShareLists);
  LOAD(wglUseFontBitmapsA);
  LOAD(wglUseFontBitmapsW);
  LOAD(wglUseFontOutlinesA);
  LOAD(wglUseFontOutlinesW);
  LOAD(wglDescribeLayerPlane);
  LOAD(wglSetLayerPaletteEntries);
  LOAD(wglGetLayerPaletteEntries);
  LOAD(wglRealizeLayerPalette);
  LOAD(wglSwapLayerBuffers);

  return status;
}

auto gl::preload() -> bool {
  bool status = true;

  if (!glGetIntegerv) {
    glGetIntegerv = reinterpret_cast<pfn_glGetIntegerv>(
        get_proc_address("glGetIntegerv"));

    if (!glGetIntegerv) {
      error(__FUNCTION__,
          "Unable to get glGetIntegerv function.");
      status = false;
    }
  }

  if (!glGetStringi) {
    glGetStringi = reinterpret_cast<pfn_glGetStringi>(
        get_proc_address("glGetStringi"));

    if (!glGetStringi) {
      error(
          __FUNCTION__, "Unable to get glGetStringi function.");
      status = false;
    }
  }

  if (glGetIntegerv && glGetStringi) {
    int32_t num_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

    gl::extensions.reserve(num_extensions);

    for (int32_t i = 0; i < num_extensions; i++) {
      auto p = reinterpret_cast<const char *>(
          glGetStringi(GL_EXTENSIONS, i));

      if (p) {
        auto name = string(p);

        gl::extensions.emplace(
            lower_bound(gl::extensions.begin(),
                gl::extensions.end(), name),
            name);
      }
    }
  }

  if (has("WGL_ARB_create_context")) {
    wglCreateContextAttribsARB =
        reinterpret_cast<pfn_wglCreateContextAttribsARB>(
            get_proc_address("wglCreateContextAttribsARB"));

    if (!wglCreateContextAttribsARB) {
      error(__FUNCTION__,
          "Unable to get wglCreateContextAttribsARB function.");
      status = false;
    }
  }

  return status;
}

void gl::platform_cleanup() {
  if (opengl_dll) {
    FreeLibrary(opengl_dll);
    opengl_dll = nullptr;
  }
}

auto gl::get_proc_address(const char *name)
    -> gl::ptr_function {
  if (gl::wglGetProcAddress) {
    if (auto p = gl::wglGetProcAddress(name); p)
      return p;
  }

  return reinterpret_cast<ptr_function>(
      GetProcAddress(opengl_dll, name));
}
