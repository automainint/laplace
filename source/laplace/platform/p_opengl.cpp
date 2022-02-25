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
#include "../core/string.h"
#include "wrap.h"
#include <algorithm>
#include <iostream>
#include <string>

#include "../.generated/gl/funcs.impl.h"

#ifdef __EMSCRIPTEN__
#  define LAPLACE_FEATURE_GL_ES_VERSION_2_0
#  define LAPLACE_FEATURE_GL_ES_VERSION_3_0
#  define LAPLACE_FEATURE_GL_ES_VERSION_3_1
#  define LAPLACE_FEATURE_GL_ES_VERSION_3_2
#else
#  define LAPLACE_FEATURE_GL_VERSION_1_0
#  define LAPLACE_FEATURE_GL_VERSION_1_1
#  define LAPLACE_FEATURE_GL_VERSION_1_2
#  define LAPLACE_FEATURE_GL_VERSION_1_3
#  define LAPLACE_FEATURE_GL_VERSION_1_4
#  define LAPLACE_FEATURE_GL_VERSION_1_5
#  define LAPLACE_FEATURE_GL_VERSION_2_0
#  define LAPLACE_FEATURE_GL_VERSION_2_1
#  define LAPLACE_FEATURE_GL_VERSION_3_0
#  define LAPLACE_FEATURE_GL_VERSION_3_1
#  define LAPLACE_FEATURE_GL_VERSION_3_2
#  define LAPLACE_FEATURE_GL_VERSION_3_3
#  define LAPLACE_FEATURE_GL_VERSION_4_0
#  define LAPLACE_FEATURE_GL_VERSION_4_1
#  define LAPLACE_FEATURE_GL_VERSION_4_2
#  define LAPLACE_FEATURE_GL_VERSION_4_3
#  define LAPLACE_FEATURE_GL_VERSION_4_4
#  define LAPLACE_FEATURE_GL_VERSION_4_5
#  define LAPLACE_FEATURE_GL_VERSION_4_6
#endif

namespace laplace::gl {
  using std::find, std::lower_bound, std::string, std::string_view,
      platform::gl_get_proc_address;

  auto ok         = false;
  auto extensions = sl::vector<string> {};

  auto is_ok() noexcept -> bool {
    return ok;
  }

  void require_extensions(sl::vector<string_view> exts) noexcept {
    extensions.insert(extensions.end(), exts.begin(), exts.end());
  }

  auto load(auto fn, string_view name, log_handler log) noexcept
      -> bool {
    if (fn != nullptr)
      return true;
    if (fn = reinterpret_cast<decltype(fn)>(
            gl_get_proc_address(name.data()));
        fn != nullptr)
      return true;
    log(log_event::fatal,
        fmt("Unable to load `%s` function.", name.data()),
        __FUNCTION__);
    return false;
  };

  auto save_extension(string_view name, bool status) noexcept {
    if (status)
      return;
    auto i = lower_bound(extensions.begin(), extensions.end(), name);
    if (i != extensions.end() && *i == name)
      extensions.erase(i);
    ok = false;
  };

  auto load_functions(log_handler log) noexcept -> bool {
    ok = true;
    sort(extensions.begin(), extensions.end());

#include "../.generated/gl/loads.impl.h"

    return ok;
  }

  auto has_extension(string_view extension) noexcept -> bool {
    auto i = lower_bound(extensions.begin(), extensions.end(),
                         extension);
    return i != extensions.end() && *i == extension;
  }
}
