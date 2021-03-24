/*  laplace/render/model.h
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

#ifndef laplace_render_model_h
#define laplace_render_model_h

#include "../core/element.h"
#include "material.h"
#include "track.h"
#include <memory>
#include <vector>

namespace laplace::render {
  class model : core::element {
  public:
    struct bind {
      size_t     node_id;
      real_track factor;
    };

    using vbind = std::vector<bind>;

    struct node {
      vec3_track position;
      quat_track rotation;
      vbind      look_at;
      vbind      binds;
    };

    struct vertex {
      vec3_track position;
      vec2_track texcoord;
      vec3_track tangent;
      vec3_track binormal;
      vec3_track normal;
      vbind      binds;
    };

    using vvertex      = std::vector<vertex>;
    using ptr_material = std::shared_ptr<material>;

    struct bunch {
      bool         opaque     : 1;
      bool         smooth     : 1;
      bool         shadow     : 1;
      bool         reflection : 1;
      bool         refraction : 1;
      bool         emission   : 1;
      ptr_material material;
      vvertex      vertices;
      vuint        indices;
      vbind        binds;
    };

    struct light {
      vec3_track            color;
      vec3_track            position;
      vec3_track            forward;
      vec3_track            up;
      real_track            fov_angle;
      real_track            intensity;
      real_track            diffuse;
      real_track            specular;
      real_track            focus;
      graphics::ptr_texture lightmap;
      vbind                 binds;
    };

    using vnode  = std::vector<node>;
    using vbunch = std::vector<bunch>;
    using vlight = std::vector<light>;

    using ref_vnode  = vnode &;
    using ref_vbunch = vbunch &;
    using ref_vlight = vlight &;

    using cref_vnode  = const vnode &;
    using cref_vbunch = const vbunch &;
    using cref_vlight = const vlight &;

    vnode  nodes;
    vbunch bunches;
    vlight lights;

    model();
    ~model() override = default;

    void clear();
  };

  using ref_model  = model &;
  using cref_model = const model &;
  using ptr_model  = std::shared_ptr<model>;
}

#endif
