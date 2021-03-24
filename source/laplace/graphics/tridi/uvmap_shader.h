/*  laplace/graphics/tridi/uvmap_shader.h
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

#ifndef laplace_graphics_tridi_uvmap_shader_h
#define laplace_graphics_tridi_uvmap_shader_h

#include "mesh_shader.h"

namespace laplace::graphics::tridi {
  class uvmap_shader {
  public:
    using vertex = mesh_shader::vertex;

    using cref_object   = mesh_shader::cref_object;
    using cref_material = mesh_shader::cref_material;
    using cref_light    = mesh_shader::cref_light;

    struct view {
      vec3 center_position;
    };

    using cref_view = view &;

    uvmap_shader(const uvmap_shader &) = delete;
    auto operator=(const uvmap_shader &) -> uvmap_shader & = delete;

    uvmap_shader(ref_istream vert, ref_istream frag);
    ~uvmap_shader();

    void use();

    void set_object(cref_object obj);
    void set_view(cref_view view);
    void set_material(cref_material mat);
    void set_light(cref_light light);

  private:
    int n_center_position;
    int n_object_rotation;
    int n_object_transform;
    int n_light_position;
    int n_light_color;
    int n_light_ambient;
    int n_light_diffuse;
    int n_light_specular;
    int n_light_focus;
    int n_material_colormap;
    int n_material_ambient;
    int n_material_diffsue;
    int n_material_specular;
    int n_light_shadowmap;

    program m_program;
  };

  using ptr_uvmap_shader = std::shared_ptr<uvmap_shader>;
}

#endif
