/*  laplace/render/r_context.cpp
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

#include "../math/transform.h"
#include "../platform/opengl.h"
#include "context.h"
#include <cassert>

namespace laplace::render {
  namespace flat  = graphics::flat;
  namespace tridi = graphics::tridi;

  using std::make_shared, std::weak_ptr, std::span,
      graphics::ref_texture, flat::solid_shader,
      flat::ptr_solid_shader, flat::ptr_sprite_shader,
      tridi::ptr_mesh_shader, tridi::ptr_uvmap_shader,
      tridi::ptr_shadow_shader, tridi::ptr_reflect_shader,
      tridi::ptr_refract_shader;

  weak_ptr<context> context::m_default;

  void context::setup(ptr_solid_shader shader) {
    m_solid_shader = shader;
  }

  void context::setup(ptr_sprite_shader shader) {
    m_sprite_shader = shader;
  }

  void context::setup(ptr_mesh_shader shader) {
    m_mesh_shader = shader;
  }

  void context::setup(ptr_uvmap_shader shader) {
    m_uvmap_shader = shader;
  }

  void context::setup(ptr_shadow_shader shader) {
    m_shadow_shader = shader;
  }

  void context::setup(ptr_reflect_shader shader) {
    m_reflect_shader = shader;
  }

  void context::setup(ptr_refract_shader shader) {
    m_refract_shader = shader;
  }

  void context::set_settings(cref_settings sets) {
    m_settings = sets;
  }

  void context::set_projection(cref_projection proj) {
    m_projection = proj;
  }
  void context::set_scene(ptr_scene sce) {
    m_scene = sce;
  }

  void context::render(span<const solid_vertex> vertices) {
    if (m_solid_shader) {
      m_solid_shader->use();
      m_solid_buffer.render(vertices);
    }
  }

  void context::render_strip(span<const solid_vertex> vertices) {
    if (m_solid_shader) {
      m_solid_shader->use();
      m_solid_buffer.render_strip(vertices);
    }
  }

  void context::render(span<const sprite_vertex> vertices,
                       ref_texture               tex) {
    if (m_sprite_shader) {
      m_sprite_shader->use();
      m_sprite_shader->set_texture(0);

      tex.bind_2d(0);
      m_sprite_buffer.render(vertices);
    }
  }

  void context::render_strip(span<const sprite_vertex> vertices,
                             ref_texture               tex) {
    if (m_sprite_shader) {
      m_sprite_shader->use();
      m_sprite_shader->set_texture(0);

      tex.bind_2d(0);
      m_sprite_buffer.render_strip(vertices);
    }
  }

  void context::render() { }

  auto context::get_default() -> ptr_context {
    auto result = m_default.lock();

    if (!result) {
      result    = make_shared<context>();
      m_default = result;
    }

    return result;
  }
}
