/*  laplace/graphics/tridi/g3_shadow_shader.cpp
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

#include "shadow_shader.h"

namespace laplace::graphics::tridi {
  shadow_shader::shadow_shader(std::istream &vert, std::istream &frag) {
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);
  }

  shadow_shader::~shadow_shader() { }

  void shadow_shader::use() {
    m_program.use(true);
  }
}
