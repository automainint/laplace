/*  laplace/render/light.h
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

#ifndef laplace_render_light_h
#define laplace_render_light_h

#include "../graphics/texture.h"
#include "defs.h"

namespace laplace::render {
  struct light {
    vec3 color     = { 1.f, 1.f, 1.f };
    vec3 position  = { 0.f, 0.f, 0.f };
    vec3 forward   = { 1.f, 0.f, 0.f };
    vec3 up        = { 0.f, 0.f, 1.f };
    real fov_angle = 0.f;
    real intensity = 1.f;
    real diffuse   = 1.f;
    real specular  = 1.f;
    real focus     = 1.f;

    graphics::ptr_texture lightmap;
  };

  using vlight     = std::vector<light>;
  using cref_light = const light &;
}

#endif
