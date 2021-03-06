/*  laplace/render/instance.h
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

#ifndef laplace_render_instance_h
#define laplace_render_instance_h

#include "mesh.h"

namespace laplace::render {
  struct instance {
    vec3     position = zero<vec3>;
    quat     rotation = zero<quat>;
    ptr_mesh object;
  };

  using ptr_instance  = std::shared_ptr<instance>;
  using vptr_instance = std::vector<ptr_instance>;
}

#endif
