/*  apps/quadwar/view/landscape.h
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

#ifndef quadwar_view_landscape_h
#define quadwar_view_landscape_h

#include "../../../laplace/graphics/flat/solid_shader.h"
#include "camera.h"
#include "defs.h"

namespace quadwar_app::view {
  class landscape : engine::helper {
  public:
    static const real tile_border;

    void render(const camera &cam, world w);

  private:
    using vertex = graphics::flat::solid_shader::vertex;

    sl::index m_state_version = 0;
    sl::whole m_width         = 0;
    sl::whole m_height        = 0;

    std::vector<vertex> m_vertices;
  };
}

#endif
