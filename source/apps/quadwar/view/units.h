/*  apps/quadwar/view/units.h
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

#ifndef quadwar_view_units_h
#define quadwar_view_units_h

#include "../../../laplace/graphics/flat/solid_shader.h"
#include "camera.h"
#include "defs.h"

namespace quadwar_app::view {
  class units : engine::helper {
  public:
    static constexpr auto default_color = vec4 { .8f, .1f, .1f, 1.f };

    void render(const camera &cam, world w);

  private:
    using vertex = graphics::flat::solid_shader::vertex;

    std::vector<vertex> m_vertices;
  };
}

#endif
