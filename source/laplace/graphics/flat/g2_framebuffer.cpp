/*  laplace/graphics/flat/g2_framebuffer.cpp
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
#include "framebuffer.h"

namespace laplace::graphics::flat {
  using namespace gl;
  using std::function;

  framebuffer::framebuffer() {
    m_width  = 0;
    m_height = 0;

    glGenFramebuffers(1, &m_id);

    glBindTexture(GL_TEXTURE_2D, color_texture.get_id());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  framebuffer::~framebuffer() {
    glDeleteFramebuffers(1, &m_id);
  }

  void framebuffer::set_size(size_t width, size_t height) {
    if (m_width != static_cast<uint32_t>(width) ||
        m_height != static_cast<uint32_t>(height)) {

      m_width  = static_cast<uint32_t>(width);
      m_height = static_cast<uint32_t>(height);

      glBindFramebuffer(GL_FRAMEBUFFER, m_id);

      glBindTexture(GL_TEXTURE_2D, color_texture.get_id());

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                   static_cast<GLsizei>(m_width),
                   static_cast<GLsizei>(m_height), 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, nullptr);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, color_texture.get_id(), 0);

      uint32_t draw[] = { GL_COLOR_ATTACHMENT0 };
      glDrawBuffers(1, draw);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }

  void framebuffer::render(function<void()> op) {
    if (op) {
      glBindFramebuffer(GL_FRAMEBUFFER, m_id);
      glViewport(0, 0, static_cast<GLsizei>(m_width),
                 static_cast<GLsizei>(m_height));

      op();

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }
}
