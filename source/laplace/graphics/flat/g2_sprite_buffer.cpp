/*  laplace/graphics/flat/g2_sprite_buffer.cpp
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
#include "sprite_buffer.h"

namespace laplace::graphics::flat {
  using namespace gl;
  using std::span;

  sprite_buffer::sprite_buffer() {
    glGenBuffers(1, &m_id);
  }

  sprite_buffer::~sprite_buffer() {
    glDeleteBuffers(1, &m_id);
  }

  void sprite_buffer::render(span<const vertex> vertices) {
    render_internal(vertices, GL_TRIANGLES);
  }

  void sprite_buffer::render_strip(span<const vertex> vertices) {
    render_internal(vertices, GL_TRIANGLE_STRIP);
  }

  void sprite_buffer::render_internal(span<const vertex> vertices,
                                      uint32_t           mode) {

    glBindBuffer(GL_ARRAY_BUFFER, m_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(vertex)),
        vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void *>(offsetof(vertex, position)));

    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void *>(offsetof(vertex, texcoord)));

    glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
  }
}
