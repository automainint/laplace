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

#include "units.h"

#include "../../../laplace/core/utils.h"
#include "../../../laplace/render/context.h"
#include "../object/root.h"
#include "../object/unit.h"

namespace quadwar_app::view {
  using object::root, object::unit, std::lower_bound, std::span;

  void units::render(const camera &cam, world w,
                     span<const sl::index> highlight,
                     span<const sl::index> selection) {

    update_units(cam, w);
    update_buffer(highlight, selection);

    auto cont = render::context::get_default();

    if (cont) {
      const auto [p, s] = cam.get_transform();
      cont->render(m_vertices, p, s);
    }
  }

  auto units::get_units() const -> span<const unit_info> {
    return m_info;
  }

  void units::update_units(const camera &cam, world w) {
    const auto r     = w.get_entity(w.get_root());
    const auto units = w.get_entity(root::get_units(r));

    m_info.resize(units.vec_get_size());

    for (sl::index i = 0; i < m_info.size(); i++) {
      const auto id = as_index(units.vec_get(i, -1));
      const auto u  = w.get_entity(id);

      const auto p = unit::get_position_scaled(u) *
                     cam.get_grid_scale();

      const auto s = unit::get_radius_scaled(u) *
                     cam.get_grid_scale() * unit_scaling;

      m_info[i].id      = id;
      m_info[i].rect[0] = p - vec2 { s, s };
      m_info[i].rect[1] = p + vec2 { s, s };
    }
  }

  void units::update_buffer(span<const sl::index> highlight,
                            span<const sl::index> selection) {

    m_vertices.resize(m_info.size() * 6,
                      vertex { vec2 {}, default_color });

    for (sl::index i = 0; i < m_info.size(); i++) {
      const auto x0 = m_info[i].rect[0].x();
      const auto y0 = m_info[i].rect[0].y();
      const auto x1 = m_info[i].rect[1].x();
      const auto y1 = m_info[i].rect[1].y();

      m_vertices[i * 6].position     = vec2 { x0, y0 };
      m_vertices[i * 6 + 1].position = vec2 { x1, y0 };
      m_vertices[i * 6 + 2].position = vec2 { x0, y1 };
      m_vertices[i * 6 + 3].position = vec2 { x1, y0 };
      m_vertices[i * 6 + 4].position = vec2 { x1, y1 };
      m_vertices[i * 6 + 5].position = vec2 { x0, y1 };

      const auto color = [id = m_info[i].id, &highlight,
                          &selection]() -> vec4 {
        if (auto i = lower_bound(highlight.begin(), highlight.end(), id);
            i != highlight.end() && *i == id)
          return highlight_color;

        if (auto i = lower_bound(selection.begin(), selection.end(), id);
            i != selection.end() && *i == id)
          return selection_color;

        return default_color;
      }();

      m_vertices[i * 6].color     = color;
      m_vertices[i * 6 + 1].color = color;
      m_vertices[i * 6 + 2].color = color;
      m_vertices[i * 6 + 3].color = color;
      m_vertices[i * 6 + 4].color = color;
      m_vertices[i * 6 + 5].color = color;
    }
  }
}
