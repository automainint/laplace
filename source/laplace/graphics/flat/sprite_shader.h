/*  laplace/graphics/flat/sprite_shader.h
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

#ifndef laplace_graphics_flat_sprite_shader_h
#define laplace_graphics_flat_sprite_shader_h

#include "../defs.h"
#include "../program.h"

namespace laplace::graphics::flat {
  class sprite_shader {
  public:
    struct vertex {
      vec2 position;
      vec2 texcoord;
    };

    sprite_shader(const sprite_shader &) = delete;
    auto operator=(const sprite_shader &) -> sprite_shader & = delete;

    sprite_shader(ref_istream vert, ref_istream frag);

    void use();

    void set_view_position(cref_vec2 position);
    void set_view_scale(cref_vec2 scale);
    void set_mesh_position(cref_vec2 position);
    void set_mesh_scale(cref_vec2 scale);
    void set_texture(size_t id);

  private:
    int n_view_position;
    int n_view_scale;
    int n_mesh_position;
    int n_mesh_scale;
    int n_texture;

    program m_program;
  };

  using ptr_sprite_shader = std::shared_ptr<sprite_shader>;
}

#endif
