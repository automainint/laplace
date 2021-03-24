/*  laplace/graphics/tridi/reflect_shader.h
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

#ifndef laplace_graphics_tridi_reflect_shader_h
#define laplace_graphics_tridi_reflect_shader_h

#include "mesh_shader.h"

namespace laplace::graphics::tridi {
  class reflect_shader {
  public:
    using vertex = mesh_shader::vertex;

    using cref_view     = mesh_shader::cref_view;
    using cref_material = mesh_shader::cref_material;

    reflect_shader(const reflect_shader &) = delete;
    auto operator=(const reflect_shader &) -> reflect_shader & = delete;

    reflect_shader(ref_istream vert, ref_istream frag);
    ~reflect_shader();

    void use();

    void set_view(cref_view view);
    void set_material(cref_material mat);

  private:
    int n_full_transform;
    int n_material_colormap;

    program m_program;
  };

  using ptr_reflect_shader = std::shared_ptr<reflect_shader>;
}

#endif
