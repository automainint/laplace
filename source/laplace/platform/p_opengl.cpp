/*  laplace/platform/p_opengl.cpp
 *
 *      OpenGL functions loading.
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

#include "../core/defs.h"
#include "opengl.h"
#include <algorithm>
#include <iostream>
#include <string>

#include "../../generated/gl/funcs.impl.h"

#define LAPLACE_GL_LOAD(a)                                  \
  if (!gl::a) {                                             \
    if (gl::a = reinterpret_cast<gl::pfn_##a>(              \
            gl::get_proc_address(#a));                      \
        !gl::a) {                                           \
      error(__FUNCTION__, "Unable to get %s function." #a); \
      gl::ok = false;                                       \
    }                                                       \
  }

#define LAPLACE_GL_LOAD_EX(a)                                \
  if (!gl::a) {                                              \
    if (gl::a = reinterpret_cast<gl::pfn_##a>(               \
            gl::get_proc_address(#a));                       \
        !gl::a) {                                            \
      error(__FUNCTION__, "Unable to get %s function.", #a); \
      status = false;                                        \
    }                                                        \
  }

#define LAPLACE_GL_HAS(a) gl::has(#a)

#define LAPLACE_GL_BEGIN_EX() \
  { status = true; }

#define LAPLACE_GL_END_EX(x)                                  \
  {                                                           \
    if (!status)                                              \
      error(                                                  \
          __FUNCTION__, "%s OpenGL extension disabled.", #x); \
    auto i = find(                                            \
        gl::extensions.begin(), gl::extensions.end(), #x);    \
    if (i != gl::extensions.end()) {                          \
      gl::extensions.erase(i);                                \
    }                                                         \
  }

namespace laplace::gl {
  bool                     ok                      = false;
  bool                     has_extensions_required = false;
  std::vector<std::string> extensions;
  std::vector<std::string> extensions_required;
}

using namespace laplace;
using namespace std;

auto gl::is_ok() -> bool {
  return gl::ok;
}

void gl::require_extensions(vector<string_view> extensions) {
  gl::has_extensions_required = true;
  gl::extensions_required.assign(
      extensions.begin(), extensions.end());
}

auto gl::init() -> bool {
  gl::ok = true;

  if (has_extensions_required) {
    gl::extensions = gl::extensions_required;
    sort(gl::extensions.begin(), gl::extensions.end());
  }

  bool status = true;

#include "../../generated/gl/loads.impl.h"

  if (has_extensions_required && !status) {
    gl::ok = false;
  }

  return gl::ok;
}

auto gl::has(std::string_view extension) -> bool {
  auto i = lower_bound(
      gl::extensions.begin(), gl::extensions.end(), extension);

  return i != gl::extensions.end() && *i == extension;
}
