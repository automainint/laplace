/*  laplace/graphics/utils.h
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

#ifndef laplace_graphics_utils_h
#define laplace_graphics_utils_h

#include "defs.h"

namespace laplace::graphics {
  void init();
  void viewport(int x, int y, size_t width, size_t height);
  void clear(cref_vec4 color);
  void clear_color_buffer(cref_vec4 color);

  void prepare_ui();
  void prepare_render();

  void set_blend_enabled(bool is_enabled);
}

#endif
