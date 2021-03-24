/*  laplace/graphics/tridi/refract_shader.h
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

#ifndef laplace_graphics_tridi_refract_shader_h
#define laplace_graphics_tridi_refract_shader_h

#include "mesh_shader.h"

namespace laplace::graphics::tridi {
  class refract_shader {
  public:
    using vertex = mesh_shader::vertex;

    using cref_view     = mesh_shader::cref_view;
    using cref_material = mesh_shader::cref_material;

    refract_shader(const refract_shader &) = delete;
    auto operator=(const refract_shader &) -> refract_shader & = delete;

    refract_shader(ref_istream vert, ref_istream frag);
    ~refract_shader();

    void use();

    void set_view(cref_view view);
    void set_material(cref_material mat);

  private:
    int n_full_transform;
    int n_material_colormap;

    program m_program;
  };

  using ptr_refract_shader = std::shared_ptr<refract_shader>;
}

#endif
