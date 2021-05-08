/*  apps/quadwar/view/aqv_units.cpp
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

#include "../../../laplace/render/context.h"
#include "../object/root.h"
#include "../object/unit.h"
#include "units.h"

namespace quadwar_app::view {
  using object::root, object::unit;

  void units::render(const camera &cam, world w) {
    const auto r     = w.get_entity(w.get_root());
    const auto units = root::get_all_units(r);

    m_vertices.resize(
        units.size() * 6, vertex { vec2 {}, default_color });

    for (sl::index i = 0; i < units.size(); i++) {
      const auto u = w.get_entity(units[i]);

      const auto p = unit::get_position_scaled(u) *
                     cam.get_grid_scale();
      const auto s = unit::get_radius_scaled(u) * cam.get_grid_scale();

      const auto p0 = p - vec2 { s, s };
      const auto d  = vec2 { s * 2, s * 2 };
      const auto dx = vec2 { s * 2, 0 };
      const auto dy = vec2 { 0, s * 2 };

      m_vertices[i * 6].position     = p0;
      m_vertices[i * 6 + 1].position = p0 + dx;
      m_vertices[i * 6 + 2].position = p0 + dy;
      m_vertices[i * 6 + 3].position = p0 + dx;
      m_vertices[i * 6 + 4].position = p0 + d;
      m_vertices[i * 6 + 5].position = p0 + dy;
    }

    auto cont = render::context::get_default();

    if (cont) {
      const auto [p, s] = cam.get_transform();
      cont->render(m_vertices, p, s);
    }
  }
}
