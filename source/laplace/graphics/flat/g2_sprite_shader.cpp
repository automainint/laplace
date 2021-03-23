/*  laplace/graphics/flat/g2_sprite_shader.cpp
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
#include "sprite_shader.h"

namespace laplace::graphics::flat {
  using namespace gl;

  sprite_shader::sprite_shader(ref_istream vert, ref_istream frag) {
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

    n_texture = glGetUniformLocation( //
        m_program.get_id(),           //
        "mesh_texture");
  }
  
  void sprite_shader::use() {
    m_program.use(true);
  }

  void sprite_shader::set_view_position(cref_vec2 position) {
    glUniform2fv(n_view_position, 1, position.v);
  }

  void sprite_shader::set_view_scale(cref_vec2 scale) {
    glUniform2fv(n_view_scale, 1, scale.v);
  }

  void sprite_shader::set_mesh_position(cref_vec2 position) {
    glUniform2fv(n_mesh_position, 1, position.v);
  }

  void sprite_shader::set_mesh_scale(cref_vec2 scale) {
    glUniform2fv(n_mesh_scale, 1, scale.v);
  }

  void sprite_shader::set_texture(size_t id) {
    glUniform1i(n_texture, static_cast<int32_t>(id));
  }
}
