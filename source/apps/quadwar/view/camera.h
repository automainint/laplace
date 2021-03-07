/*  apps/quadwar/view/camera.h
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

#ifndef quadwar_view_camera_h
#define quadwar_view_camera_h

#include "defs.h"

namespace quadwar_app::view {
  class camera {
  public:
    void set_frame(const vec2 size) noexcept;
    void set_bounds(const vec2 v0, const vec2 v1) noexcept;
    void set_scale(const real scale) noexcept;
    void set_position(const vec2 v) noexcept;

    [[nodiscard]] auto get_position() const noexcept -> vec2;
    [[nodiscard]] auto get_frame() const noexcept -> vec2;
    [[nodiscard]] auto get_scale() const noexcept -> real;

  private:
    real m_scale = 1.f;
    vec2 m_position;
    vec2 m_frame;
    vec2 m_min;
    vec2 m_max;
  };
}

#endif
