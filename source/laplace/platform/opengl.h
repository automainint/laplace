/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_PLATFORM_OPENGL_H
#define LAPLACE_PLATFORM_OPENGL_H

#include <string>
#include <vector>

#include "../.generated/gl/types.h"
#include "../core/log.h"

namespace laplace::gl {
  using ptr_function = void(LAPLACE_GL_API *)();

  auto is_ok() -> bool;

  void require_extensions(std::vector<std::string_view> extensions);

  auto load_functions(log_handler log) -> bool;

  auto has_extension(std::string_view extension) -> bool;
}

#endif
