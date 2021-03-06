/*  laplace/render/scene.h
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

#ifndef laplace_render_scene_h
#define laplace_render_scene_h

#include "camera.h"
#include "instance.h"
#include "light.h"
#include <vector>

namespace laplace::render {
  static constexpr real default_ambient_light = .1f;

  struct scene {
    real          ambient_light = default_ambient_light;
    camera        camera;
    vlight        lights;
    vptr_instance objects;
  };

  using ptr_scene = std::shared_ptr<scene>;
}

#endif
