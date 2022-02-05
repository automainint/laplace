/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "context.h"

namespace laplace::render {
  namespace flat = graphics::flat;

  using std::make_shared, std::weak_ptr, std::span,
      graphics::ref_texture, graphics::vec2, graphics::cref_vec2,
      flat::solid_shader, flat::ptr_solid_shader,
      flat::ptr_sprite_shader;

  weak_ptr<context> context::m_default;

  void context::setup(ptr_solid_shader shader) noexcept {
    m_solid_shader = shader;
  }

  void context::setup(ptr_sprite_shader shader) noexcept {
    m_sprite_shader = shader;
  }

  void context::adjust_frame_size(sl::whole width,
                                  sl::whole height) noexcept {
    auto const x0 = width < 0 ? 1.f : -1.f;
    auto const y0 = height < 0 ? -1.f : 1.f;
    auto const w  = 2.f / static_cast<float>(width);
    auto const h  = -2.f / static_cast<float>(height);

    if (m_solid_shader) {
      m_solid_shader->use();
      m_solid_shader->set_view_position({ x0, y0 });
      m_solid_shader->set_view_scale({ w, h });
    }

    if (m_sprite_shader) {
      m_sprite_shader->use();
      m_sprite_shader->set_view_position({ x0, y0 });
      m_sprite_shader->set_view_scale({ w, h });
    }
  }

  void context::render(span<solid_vertex const> vertices) noexcept {
    this->render(vertices, vec2 { 0.f, 0.f }, vec2 { 1.f, 1.f });
  }

  void context::render(span<solid_vertex const> vertices,
                       cref_vec2 position, cref_vec2 scale) noexcept {

    if (!m_solid_shader)
      return;

    m_solid_shader->use();
    m_solid_shader->set_mesh_position(position);
    m_solid_shader->set_mesh_scale(scale);
    m_solid_buffer.render(vertices);
  }

  void context::render_strip(
      span<solid_vertex const> vertices) noexcept {
    if (!m_solid_shader)
      return;

    m_solid_shader->use();
    m_solid_shader->set_mesh_position({ 0.f, 0.f });
    m_solid_shader->set_mesh_scale({ 1.f, 1.f });
    m_solid_buffer.render_strip(vertices);
  }

  void context::render(span<sprite_vertex const> vertices,
                       ref_texture               tex) noexcept {

    this->render(vertices, vec2 { 0.f, 0.f }, vec2 { 1.f, 1.f }, tex);
  }

  void context::render(span<sprite_vertex const> vertices,
                       cref_vec2 position, cref_vec2 scale,
                       ref_texture tex) noexcept {

    if (!m_sprite_shader)
      return;

    m_sprite_shader->use();
    m_sprite_shader->set_mesh_position(position);
    m_sprite_shader->set_mesh_scale(scale);
    m_sprite_shader->set_texture(0);

    tex.bind_2d(0);
    m_sprite_buffer.render(vertices);
  }

  void context::render_strip(span<sprite_vertex const> vertices,
                             ref_texture               tex) noexcept {

    if (!m_sprite_shader)
      return;

    m_sprite_shader->use();
    m_sprite_shader->set_mesh_position({ 0.f, 0.f });
    m_sprite_shader->set_mesh_scale({ 1.f, 1.f });
    m_sprite_shader->set_texture(0);

    tex.bind_2d(0);
    m_sprite_buffer.render_strip(vertices);
  }

  auto context::get_default() noexcept -> ptr_context {
    auto result = m_default.lock();

    if (!result) {
      result    = make_shared<context>();
      m_default = result;
    }

    return result;
  }
}
