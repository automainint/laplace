/*  laplace/graphics/flat/solid_buffer.h
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

#ifndef laplace_graphics_flat_solid_buffer_h
#define laplace_graphics_flat_solid_buffer_h

#include "solid_shader.h"

namespace laplace::graphics::flat {
  class solid_buffer {
  public:
    using vertex = solid_shader::vertex;

    solid_buffer();
    ~solid_buffer();

    void render(std::span<const vertex> vertices);
    void render_strip(std::span<const vertex> vertices);

  private:
    void render_internal(std::span<const vertex> vertices,
                         uint32_t                mode);

    uint32_t m_id;
  };
}

#endif
