/*  laplace/graphics/flat/g2_solid_shader.cpp
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
#include "solid_shader.h"

namespace laplace::graphics::flat {
  using namespace gl;

  solid_shader::solid_shader(std::istream &vert, std::istream &frag) {
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_view_position = glGetUniformLocation( //
        m_program.get_id(),                 //
        "view_position");

    n_view_scale = glGetUniformLocation( //
        m_program.get_id(),              //
        "view_scale");

    n_mesh_position = glGetUniformLocation( //
        m_program.get_id(),                 //
        "mesh_position");

    n_mesh_scale = glGetUniformLocation( //
        m_program.get_id(),              //
        "mesh_scale");
  }

  void solid_shader::use() {
    m_program.use(true);
  }

  void solid_shader::set_view_position(cref_vec2 position) {
    glUniform2fv(n_view_position, 1, position.v);
  }

  void solid_shader::set_view_scale(cref_vec2 scale) {
    glUniform2fv(n_view_scale, 1, scale.v);
  }

  void solid_shader::set_mesh_position(cref_vec2 position) {
    glUniform2fv(n_mesh_position, 1, position.v);
  }

  void solid_shader::set_mesh_scale(cref_vec2 scale) {
    glUniform2fv(n_mesh_scale, 1, scale.v);
  }
}
