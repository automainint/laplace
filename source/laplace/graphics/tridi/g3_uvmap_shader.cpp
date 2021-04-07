/*  laplace/graphics/tridi/g3_uvmap_shader.cpp
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
#include "uvmap_shader.h"

namespace laplace::graphics::tridi {
  uvmap_shader::uvmap_shader(std::istream &vert, std::istream &frag) {
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_center_position = gl::glGetUniformLocation(
        m_program.get_id(), "center_position");
    n_object_rotation = gl::glGetUniformLocation(
        m_program.get_id(), "object_rotation");
    n_object_transform = gl::glGetUniformLocation(
        m_program.get_id(), "object_transform");
    n_light_position = gl::glGetUniformLocation(
        m_program.get_id(), "light_position");
    n_light_color = gl::glGetUniformLocation(
        m_program.get_id(), "light_color");
    n_light_ambient = gl::glGetUniformLocation(
        m_program.get_id(), "light_ambient");
    n_light_diffuse = gl::glGetUniformLocation(
        m_program.get_id(), "light_diffuse");
    n_light_specular = gl::glGetUniformLocation(
        m_program.get_id(), "light_specular");
    n_light_focus = gl::glGetUniformLocation(
        m_program.get_id(), "light_focus");
    n_material_colormap = gl::glGetUniformLocation(
        m_program.get_id(), "material_colormap");
    n_material_ambient = gl::glGetUniformLocation(
        m_program.get_id(), "material_ambient");
    n_material_diffsue = gl::glGetUniformLocation(
        m_program.get_id(), "material_diffsue");
    n_material_specular = gl::glGetUniformLocation(
        m_program.get_id(), "material_specular");
    n_light_shadowmap = gl::glGetUniformLocation(
        m_program.get_id(), "light_shadowmap");
  }

  uvmap_shader::~uvmap_shader() { }

  void uvmap_shader::use() {
    m_program.use(true);
  }

  void uvmap_shader::set_object(uvmap_shader::cref_object obj) {
    gl::glUniformMatrix3fv(n_object_rotation, 1, false, obj.rotation.v);
    gl::glUniformMatrix4fv(
        n_object_transform, 1, false, obj.transform.v);
  }

  void uvmap_shader::set_view(uvmap_shader::cref_view view) {
    gl::glUniform3fv(n_center_position, 1, view.center_position.v);
  }

  void uvmap_shader::set_material(uvmap_shader::cref_material mat) {
    gl::glUniform1i(
        n_material_colormap, static_cast<int32_t>(mat.colormap_id));
    gl::glUniform1f(n_material_diffsue, mat.diffuse);
    gl::glUniform1f(n_material_specular, mat.specular);
  }

  void uvmap_shader::set_light(uvmap_shader::cref_light light) {
    gl::glUniform1i(
        n_light_shadowmap, static_cast<int32_t>(light.shadowmap_id));
    gl::glUniform3fv(n_light_position, 1, light.position.v);
    gl::glUniform3fv(n_light_color, 1, light.color.v);
    gl::glUniform1f(n_light_ambient, light.ambient);
    gl::glUniform1f(n_light_diffuse, light.diffuse);
    gl::glUniform1f(n_light_specular, light.specular);
    gl::glUniform1f(n_light_focus, light.focus);
  }
}
