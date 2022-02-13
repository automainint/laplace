/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "opengl.h"

#include "../core/defs.h"
#include "wrap.h"
#include <algorithm>
#include <iostream>
#include <string>

#include "../.generated/gl/funcs.impl.h"

#define LAPLACE_GL_LOAD(a)                                      \
  if (!a) {                                                     \
    if (a = reinterpret_cast<pfn_##a>(gl_get_proc_address(#a)); \
        !a) {                                                   \
      error_("Unable to load " #a " function.", __FUNCTION__);  \
      ok = false;                                               \
    }                                                           \
  }

#define LAPLACE_GL_LOAD_EX(a)                                   \
  if (!a) {                                                     \
    if (a = reinterpret_cast<pfn_##a>(gl_get_proc_address(#a)); \
        !a) {                                                   \
      error_("Unable to load " #a " function.", __FUNCTION__);  \
      status = false;                                           \
    }                                                           \
  }

#define LAPLACE_GL_HAS(a) has_extension(#a)

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
  using std::find, std::lower_bound, std::string, std::string_view;

  auto ok                      = false;
  auto has_extensions_required = false;

  auto extensions          = sl::vector<string> {};
  auto extensions_required = sl::vector<string> {};

  auto is_ok() -> bool {
    return ok;
  }

  void require_extensions(sl::vector<string_view> exts) {
    extensions_required.insert(extensions_required.end(),
                               exts.begin(), exts.end());

    has_extensions_required = !extensions_required.empty();
  }

  auto load_functions() -> bool {
    using platform::gl_get_proc_address;

    ok = true;

    if (has_extensions_required) {
      extensions = extensions_required;
      sort(extensions.begin(), extensions.end());
    }

    auto status = true;

#include "../.generated/gl/loads.impl.h"

    if (has_extensions_required && !status)
      ok = false;

    return ok;
  }

  auto has_extension(string_view extension) -> bool {
    auto i = lower_bound(extensions.begin(), extensions.end(),
                         extension);

    return i != extensions.end() && *i == extension;
  }
}
