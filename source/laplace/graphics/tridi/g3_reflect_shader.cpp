/*  laplace/graphics/tridi/g3_reflect_shader.cpp
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

#include "../../platform/gldef.h"
#include "reflect_shader.h"

namespace laplace::graphics::tridi {
  reflect_shader::reflect_shader(std::istream &vert,
                                 std::istream &frag) {
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_full_transform = gl::glGetUniformLocation(
        m_program.get_id(), "full_transform");
    n_material_colormap = gl::glGetUniformLocation(
        m_program.get_id(), "material_colormap");
  }

  reflect_shader::~reflect_shader() { }

  void reflect_shader::use() {
    m_program.use(true);
  }

  void reflect_shader::set_view(reflect_shader::cref_view view) {
    gl::glUniformMatrix4fv(
        n_full_transform, 1, false, view.full_transform.v);
  }

  void reflect_shader::set_material(reflect_shader::cref_material mat) {
    gl::glUniform1i(
        n_material_colormap, static_cast<int32_t>(mat.colormap_id));
  }
}
