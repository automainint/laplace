/*  laplace/graphics/flat/g2_solid_buffer.cpp
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
#include "solid_buffer.h"

namespace laplace::graphics::flat {
  using namespace gl;
  using std::span;

  solid_buffer::solid_buffer() {
    glGenBuffers(1, &m_id);
  }

  solid_buffer::~solid_buffer() {
    glDeleteBuffers(1, &m_id);
  }

  void solid_buffer::render(span<const vertex> vertices) {
    render_internal(vertices, GL_TRIANGLES);
  }

  void solid_buffer::render_strip(span<const vertex> vertices) {
    render_internal(vertices, GL_TRIANGLE_STRIP);
  }

  void solid_buffer::render_internal(span<const vertex> vertices,
                                     uint32_t           mode) {

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex),
                 vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void *>(offsetof(vertex, position)));

    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void *>(offsetof(vertex, color)));

    glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
  }
}
