/*  apps/quadwar/view/aqv_camera.cpp
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

#include "camera.h"

namespace quadwar_app::view {
  using std::min, std::max;

  void camera::set_grid_scale(const real scale) noexcept {
    m_grid_scale = scale;
  }

  void camera::set_frame(const vec2 size) noexcept {
    m_frame = size;
  }

  void camera::set_bounds(const vec2 v0, const vec2 v1) noexcept {
    m_min = v0;
    m_max = v1;

    adjust_position(m_position_origin);
  }

  void camera::set_scale(const real scale) noexcept {
    m_scale = scale;
  }

  void camera::set_position(const vec2 v) noexcept {
    m_position_origin = v;
    adjust_position(v);
  }

  void camera::adjust_position(const vec2 v) noexcept {
    m_position.x() = max(m_min.x(), min(v.x(), m_max.x()));
    m_position.y() = max(m_min.y(), min(v.y(), m_max.y()));
  }

  auto camera::get_grid_scale() const noexcept -> real {
    return m_grid_scale;
  }

  auto camera::get_position() const noexcept -> vec2 {
    return m_position;
  }

  auto camera::get_frame() const noexcept -> vec2 {
    return m_frame;
  }

  auto camera::get_scale() const noexcept -> real {
    return m_scale;
  }

  auto camera::get_transform() const noexcept -> camera::transform {
    const auto s = get_scale();
    const auto f = get_frame() / 2.f;
    const auto p = -get_position();

    return { f + p * s, { s, s } };
  }
}
