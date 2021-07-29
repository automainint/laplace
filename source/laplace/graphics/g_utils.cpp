/*  laplace/graphics/g_utils.cpp
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

#include "../platform/gldef.h"
#include "utils.h"

namespace laplace::graphics {
  using namespace gl;

  void init() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);
  }

  void viewport(sl::index x, sl::index y, sl::whole width,
                sl::whole height) {

    auto x0 = static_cast<GLint>(x);
    auto y0 = static_cast<GLint>(y);
    auto w  = static_cast<GLsizei>(width);
    auto h  = static_cast<GLsizei>(height);

    glViewport(x0, y0, w, h);
  }

  void clear(cref_vec4 color) {
    glClearColor(color[0], color[1], color[2], color[3]);
    glClearDepth(1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void clear_color_buffer(cref_vec4 color) {
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void prepare_ui() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
  }

  void prepare_render() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
  }

  void set_blend_enabled(bool is_enabled) {
    if (is_enabled) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }
}
