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
  namespace tridi = graphics::tridi;

  using std::make_shared, std::weak_ptr, tridi::ptr_mesh_shader,
      tridi::ptr_uvmap_shader, tridi::ptr_shadow_shader,
      tridi::ptr_reflect_shader, tridi::ptr_refract_shader;

  weak_ptr<context> context::m_default;

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

  void context::set_scene(ptr_scene sce) {
    m_scene = sce;
  }

  void context::set_settings(cref_settings sets) {
    m_settings = sets;
  }

  void context::set_projection(cref_projection proj) {
    m_projection = proj;
  }

  void context::render() { }

  auto context::get_default() -> ptr_context {
    ptr_context result = m_default.lock();

    if (!result) {
      result    = make_shared<context>();
      m_default = result;
    }

    return result;
  }
}
