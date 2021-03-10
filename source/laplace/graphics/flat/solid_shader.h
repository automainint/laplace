/*  laplace/graphics/flat/solid_shader.h
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

#ifndef laplace_graphics_flat_solid_shader_h
#define laplace_graphics_flat_solid_shader_h

#include "../defs.h"
#include "../program.h"

namespace laplace::graphics::flat {
  class solid_shader {
  public:
    struct vertex {
      vec2 position;
      vec4 color;
    };

    solid_shader(const solid_shader &) = delete;
    auto operator=(const solid_shader &) -> solid_shader & = delete;

    solid_shader(ref_istream vert, ref_istream frag);
    ~solid_shader();

    void use();

    void set_position(cref_vec2 position);
    void set_scale(cref_vec2 scale);

  private:
    int n_position;
    int n_scale;

    program m_program;
  };

  using ptr_solid_shader = std::shared_ptr<solid_shader>;
}

#endif
