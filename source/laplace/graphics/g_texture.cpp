/*  laplace/graphics/g_texture.cpp
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

#include "../platform/gldef.h"
#include "texture.h"

namespace laplace::graphics {
  using namespace gl;

  static constexpr GLenum gl_wrap[3] = { GL_CLAMP, GL_REPEAT,
                                         GL_MIRRORED_REPEAT };

  texture::texture() {
    static element::type type_texture;
    set_type_id(type_texture.get_id());

    m_ok      = true;
    m_wrap[0] = m_wrap[1] = m_wrap[2] = texture::clamp;
    glGenTextures(1, &m_id);
  }

  texture::~texture() {
    if (m_ok) {
      glDeleteTextures(1, &m_id);
    }
  }

  texture::texture(texture &&tex) noexcept {
    m_ok = tex.m_ok;
    m_id = tex.m_id;
    memcpy(m_wrap, tex.m_wrap, sizeof m_wrap);

    tex.m_ok = false;
  }

  auto texture::operator=(texture &&tex) noexcept -> texture & {
    if (m_ok) {
      glDeleteTextures(1, &m_id);
    }

    m_ok = tex.m_ok;
    m_id = tex.m_id;
    memcpy(m_wrap, tex.m_wrap, sizeof m_wrap);

    tex.m_ok = false;

    return *this;
  }

  void texture::bind_2d(size_t index) {
    glActiveTexture(static_cast<uint32_t>(GL_TEXTURE0 + index));
    glBindTexture(GL_TEXTURE_2D, m_id);
  }

  void texture::bind_3d(size_t index) {
    glActiveTexture(static_cast<uint32_t>(GL_TEXTURE0 + index));
    glBindTexture(GL_TEXTURE_3D, m_id);
  }

  void texture::set_wrap_x(wrap mode) {
    m_wrap[0] = static_cast<int32_t>(gl_wrap[mode]);
  }

  void texture::set_wrap_y(wrap mode) {
    m_wrap[1] = static_cast<int32_t>(gl_wrap[mode]);
  }

  void texture::set_wrap_z(wrap mode) {
    m_wrap[2] = static_cast<int32_t>(gl_wrap[mode]);
  }

  void texture::image_2d_linear(cref_image img) {
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    const auto width  = static_cast<GLsizei>(img.get_width());
    const auto height = static_cast<GLsizei>(img.get_height());

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img.get_data());
  }

  void texture::image_2d_nearest(cref_image img) {
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    const auto width  = static_cast<GLsizei>(img.get_width());
    const auto height = static_cast<GLsizei>(img.get_height());

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img.get_data());
  }

  void texture::mipmaps_2d_linear(cref_image img) {
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    for (size_t level = 0; level < img.get_mip_count(); level++) {
      const auto mip    = img.mip(level);
      const auto n      = static_cast<GLint>(level);
      const auto width  = static_cast<GLsizei>(mip->get_width());
      const auto height = static_cast<GLsizei>(mip->get_height());

      glTexImage2D(GL_TEXTURE_2D, n, 4, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, mip->get_data());
    }
  }

  void texture::mipmaps_2d_nearest(cref_image img) {
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    for (size_t level = 0; level < img.get_mip_count(); level++) {
      const auto mip    = img.mip(level);
      const auto n      = static_cast<GLint>(level);
      const auto width  = static_cast<GLsizei>(mip->get_width());
      const auto height = static_cast<GLsizei>(mip->get_height());

      glTexImage2D(GL_TEXTURE_2D, n, 4, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, mip->get_data());
    }
  }

  void texture::image_3d_linear(cref_image img) {
    glBindTexture(GL_TEXTURE_3D, m_id);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_wrap[2]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    const auto w = static_cast<GLsizei>(img.get_width());
    const auto h = static_cast<GLsizei>(img.get_height());
    const auto d = static_cast<GLsizei>(img.get_depth());

    glTexImage3D(GL_TEXTURE_3D, 0, 4, w, h, d, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img.get_data());
  }

  void texture::image_3d_nearest(cref_image img) {
    glBindTexture(GL_TEXTURE_3D, m_id);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_wrap[2]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto w = static_cast<GLsizei>(img.get_width());
    auto h = static_cast<GLsizei>(img.get_height());
    auto d = static_cast<GLsizei>(img.get_depth());

    glTexImage3D(GL_TEXTURE_3D, 0, 4, w, h, d, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img.get_data());
  }

  auto texture::get_id() const -> uint32_t {
    return m_id;
  }
}
