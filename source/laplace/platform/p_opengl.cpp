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
#include "wrap.h"
#include <algorithm>
#include <iostream>
#include <string>

#include "../../generated/gl/funcs.impl.h"

#define LAPLACE_GL_LOAD(a)                                         \
  if (!a) {                                                        \
    if (a = reinterpret_cast<pfn_##a>(get_proc_address(#a)); !a) { \
      ok = false;                                                  \
    }                                                              \
  }

#define LAPLACE_GL_LOAD_EX(a)                                      \
  if (!a) {                                                        \
    if (a = reinterpret_cast<pfn_##a>(get_proc_address(#a)); !a) { \
      status = false;                                              \
    }                                                              \
  }

#define LAPLACE_GL_HAS(a) has(#a)

#define LAPLACE_GL_BEGIN_EX() \
  { status = true; }

#define LAPLACE_GL_END_EX(x)                                 \
  {                                                          \
    auto i = find(extensions.begin(), extensions.end(), #x); \
    if (i != extensions.end()) {                             \
      extensions.erase(i);                                   \
    }                                                        \
  }

namespace laplace::gl {
  using std::find, std::lower_bound, std::vector, std::string,
      std::string_view;

  auto ok                      = false;
  auto has_extensions_required = false;

  auto extensions          = vector<string> {};
  auto extensions_required = vector<string> {};

  auto is_ok() -> bool {
    return ok;
  }

  auto get_proc_address(const char *name) -> ptr_function {
    return platform::get_proc_address(name);
  }

  void require_extensions(vector<string_view> exts) {
    has_extensions_required = true;
    extensions_required.assign(exts.begin(), exts.end());
  }

  auto init() -> bool {
    ok = true;

    if (has_extensions_required) {
      extensions = extensions_required;
      sort(extensions.begin(), extensions.end());
    }

    auto status = true;

#include "../../generated/gl/loads.impl.h"

    if (has_extensions_required && !status) {
      ok = false;
    }

    return ok;
  }

  auto has(string_view extension) -> bool {
    auto i = lower_bound(
        extensions.begin(), extensions.end(), extension);

    return i != extensions.end() && *i == extension;
  }
}
