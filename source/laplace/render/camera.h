/*  laplace/render/camera.h
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

#ifndef laplace_render_camera_h
#define laplace_render_camera_h

#include "defs.h"
#include <memory>

namespace laplace::render {
  struct camera {
    vec3 position = zero<vec3>;
    mat3 rotation = unit<mat3>;
  };

  using ref_camera  = camera &;
  using cref_camera = const camera &;
  using ptr_camera  = std::shared_ptr<camera>;
}

#endif
