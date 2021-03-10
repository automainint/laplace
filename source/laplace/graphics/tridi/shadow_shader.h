/*  laplace/graphics/tridi/shadow_shader.h
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

#ifndef laplace_graphics_tridi_shadow_shader_h
#define laplace_graphics_tridi_shadow_shader_h

#include "uvmap_shader.h"

namespace laplace::graphics::tridi {
  /*  UV depth map.
   */
  class shadow_shader {
  public:
    using vertex = mesh_shader::vertex;

    using cref_object   = mesh_shader::cref_object;
    using cref_view     = uvmap_shader::cref_view;
    using cref_material = mesh_shader::cref_material;
    using cref_light    = mesh_shader::cref_light;

    shadow_shader(const shadow_shader &) = delete;
    auto operator=(const shadow_shader &) -> shadow_shader & = delete;

    shadow_shader(ref_istream vert, ref_istream frag);
    ~shadow_shader();

    void use();

  private:
    program m_program;
  };

  using ptr_shadow_shader = std::shared_ptr<shadow_shader>;
}

#endif
