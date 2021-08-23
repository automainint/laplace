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
  using object::root, object::unit, std::find, std::span;

  const vec4 units::colors[] = { vec4 { .1f, .6f, .1f, 1.f },
                                 vec4 { .6f, .1f, .1f, 1.f },
                                 vec4 { .1f, .1f, .6f, 1.f },
                                 vec4 { .5f, .5f, .0f, 1.f } };

  const vec4 units::highlight_color = vec4 { .9f, .9f, .9f, .8f };
  const vec4 units::selection_color = vec4 { .7f, .7f, .7f, .8f };
  const real units::unit_scaling    = .7f;
  const real units::selection_delta = 8.f;

  void units::render(const camera &cam, world w,
                     span<const sl::index> highlight,
                     span<const sl::index> selection) {

    update_units(cam, w);
    update_buffer(highlight, selection);

    auto cont = render::context::get_default();

    if (cont) {
      const auto [p, s] = cam.get_transform();
      cont->render(m_marks, p, s);
      cont->render(m_units, p, s);
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

      m_info[i].id          = id;
      m_info[i].color_index = unit::get_color(u);
      m_info[i].rect[0]     = p - vec2 { s, s };
      m_info[i].rect[1]     = p + vec2 { s, s };
    }
  }

  void units::update_buffer(span<const sl::index> highlight,
                            span<const sl::index> selection) {
    static_assert(sizeof colors > 0);

    auto add_rect = [](sl::vector<vertex> &v, sl::index n,
                       const vec2 min, const vec2 max,
                       const float delta, const vec4 color) {
      if (n >= v.size()) {
        v.resize(n + 6);
      }

      const auto x0 = min.x() - delta;
      const auto y0 = min.y() - delta;
      const auto x1 = max.x() + delta;
      const auto y1 = max.y() + delta;

      v[n].position     = vec2 { x0, y0 };
      v[n + 1].position = vec2 { x1, y0 };
      v[n + 2].position = vec2 { x0, y1 };
      v[n + 3].position = vec2 { x1, y0 };
      v[n + 4].position = vec2 { x1, y1 };
      v[n + 5].position = vec2 { x0, y1 };

      v[n].color     = color;
      v[n + 1].color = color;
      v[n + 2].color = color;
      v[n + 3].color = color;
      v[n + 4].color = color;
      v[n + 5].color = color;
    };

    m_units.resize(m_info.size() * 6, vertex {});

    m_marks.clear();

    for (sl::index i = 0; i < m_info.size(); i++) {

      const auto min = m_info[i].rect[0];
      const auto max = m_info[i].rect[1];

      if (find(highlight.begin(), highlight.end(), m_info[i].id) !=
          highlight.end()) {
        add_rect(m_marks, m_marks.size(), min, max, selection_delta,
                 highlight_color);

      } else if (find(selection.begin(), selection.end(),
                      m_info[i].id) != selection.end()) {
        add_rect(m_marks, m_marks.size(), min, max, selection_delta,
                 selection_color);
      }

      const auto color_index = m_info[i].color_index %
                               (sizeof colors / sizeof *colors);
      const auto color = colors[color_index];

      add_rect(m_units, i * 6, min, max, 0.f, color);
    }
  }
}
