/*  laplace/graphics/tridi/mesh_shader.h
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

#ifndef laplace_graphics_tridi_mesh_shader_h
#define laplace_graphics_tridi_mesh_shader_h

#include "../defs.h"
#include "../program.h"

namespace laplace::graphics::tridi {
  class mesh_shader {
  public:
    struct vertex {
      vec3 position;
      vec3 normal;
      vec2 texcoord;
    };

    struct object {
      mat3 rotation;
      mat4 transform;
    };

    struct view {
      mat4 transform;
      mat4 full_transform;
      vec3 eye_position;
    };

    struct material {
      size_t colormap_id;
      real   diffuse;
      real   specular;
    };

    struct light {
      size_t shadowmap_id;
      vec3   position;
      vec3   color;
      real   ambient;
      real   diffuse;
      real   specular;
      real   focus;
    };

    using cref_object   = object &;
    using cref_view     = view &;
    using cref_material = material &;
    using cref_light    = light &;

    mesh_shader(const mesh_shader &) = delete;
    auto operator=(const mesh_shader &) -> mesh_shader & = delete;

    mesh_shader(ref_istream vert, ref_istream frag);
    ~mesh_shader();

    void use();

    void set_object(cref_object obj);
    void set_view(cref_view view);
    void set_material(cref_material mat);
    void set_light(cref_light light);

    static void prepare();
    static void blending(bool is_enabled);

  private:
    int n_object_rotation;
    int n_object_transform;
    int n_view_transform;
    int n_eye_position;
    int n_light_position;
    int n_material_colormap;
    int n_material_diffsue;
    int n_material_specular;
    int n_light_color;
    int n_light_ambient;
    int n_light_diffuse;
    int n_light_specular;
    int n_light_focus;
    int n_light_shadowmap;

    program m_program;
  };

  using ptr_mesh_shader = std::shared_ptr<mesh_shader>;
}

#endif
