/*  laplace/graphics/tridi/g3_refract_shader.cpp
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
#include "refract_shader.h"

namespace laplace::graphics::tridi {
  refract_shader::refract_shader(std::istream &vert,
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

  refract_shader::~refract_shader() { }

  void refract_shader::use() {
    m_program.use(true);
  }

  void refract_shader::set_view(refract_shader::cref_view view) {
    gl::glUniformMatrix4fv(
        n_full_transform, 1, false, view.full_transform.v);
  }

  void refract_shader::set_material(refract_shader::cref_material mat) {
    gl::glUniform1i(
        n_material_colormap, static_cast<int32_t>(mat.colormap_id));
  }
}
