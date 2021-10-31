/*  laplace/platform/linux/linux_glx.cpp
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

#include "glx.h"

#include "../../core/defs.h"
#include "../../core/string.h"
#include <dlfcn.h>

#define LOAD(a)                                                   \
  if (a = reinterpret_cast<pfn_##a>(dlsym(gl_library, #a)); !a) { \
    error_(fmt("Unable to load %s function.", #a), __FUNCTION__); \
    status = false;                                               \
  }

namespace laplace::linux {
  constexpr auto opengl_path = "libGL.so";

  void     *gl_library   = nullptr;
  sl::whole gl_ref_count = 0;

  pfn_glXGetProcAddress glXGetProcAddress = nullptr;
  pfn_glXChooseVisual   glXChooseVisual   = nullptr;
  pfn_glXSwapBuffers    glXSwapBuffers    = nullptr;
  pfn_glXCreateContext  glXCreateContext  = nullptr;
  pfn_glXMakeCurrent    glXMakeCurrent    = nullptr;
  pfn_glXDestroyContext glXDestroyContext = nullptr;

  auto gl_load_library() noexcept -> bool {
    gl_ref_count++;

    if (gl_library != nullptr)
      return true;

    gl_library = dlopen(opengl_path, RTLD_LAZY);

    if (gl_library == nullptr) {
      error_("dlopen failed.", __FUNCTION__);
      return false;
    }

    bool status = true;

    LOAD(glXGetProcAddress);
    LOAD(glXChooseVisual);
    LOAD(glXSwapBuffers);
    LOAD(glXCreateContext);
    LOAD(glXMakeCurrent);
    LOAD(glXDestroyContext);

    return status;
  }

  void gl_free_library() noexcept {
    if (--gl_ref_count > 0)
      return;

    if (gl_library != nullptr) {
      dlclose(gl_library);
      gl_library = nullptr;
    }
  }

  auto gl_get_proc_address(char const *name) noexcept
      -> gl::ptr_function {
    if (gl_library == nullptr)
      return nullptr;

    if (glXGetProcAddress != nullptr) {
      if (auto p = glXGetProcAddress(name); p != nullptr)
        return p;
    }

    return reinterpret_cast<gl::ptr_function>(
        dlsym(gl_library, name));
  }

  auto gl_is_loaded() noexcept -> bool {
    return gl_library != nullptr;
  }
}
