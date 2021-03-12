/*  apps/quadwar/view/aqv_landscape.cpp
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
#include "../object/landscape.h"
#include "../object/root.h"
#include "landscape.h"

namespace quadwar_app::view {
  using std::span, graphics::flat::solid_shader;

  void landscape::render(const camera &cam, world w) {
    auto cont = render::context::get_default();

    if (cont) {
      const auto r    = w.get_entity(w.get_root());
      const auto land = w.get_entity(object::root::get_landscape(r));

      const auto width  = object::landscape::get_width(land);
      const auto height = object::landscape::get_height(land);
      const auto tiles  = object::landscape::get_tiles(land);

      if (tiles.size() != width * height)
        return;

      const auto c_tile   = vec4 { .3f, .25f, .2f, 1.f };
      const auto c_border = vec4 { .2f, .15f, .15f, .7f };

      const auto s = cam.get_scale();

      const auto fw = static_cast<real>(width);
      const auto fh = static_cast<real>(height);

      const auto fx = (cam.get_frame().x() - tail_size * fw * s) / 2.f;
      const auto fy = (cam.get_frame().y() - tail_size * fh * s) / 2.f;

      const auto px = -cam.get_position().x();
      const auto py = -cam.get_position().y();

      m_vertices.reserve(12 * width * height);

      for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {

          if (tiles[j * width + i] == object::landscape::tile_border) {

            const auto tail_x = px + static_cast<real>(i) * tail_size;
            const auto tail_y = py + static_cast<real>(j) * tail_size;

            const auto x0 = fx + tail_x * s;
            const auto y0 = fy + tail_y * s;

            const auto x1 = x0 + tail_size * s;
            const auto y1 = y0 + tail_size * s;

            const auto x2 = x0 + tail_border * s;
            const auto y2 = y0 + tail_border * s;

            const auto x3 = x1 - tail_border * s;
            const auto y3 = y1 - tail_border * s;

            m_vertices.emplace_back(
                vertex { .position = { x0, y0 }, .color = c_border });
            m_vertices.emplace_back(
                vertex { .position = { x1, y0 }, .color = c_border });
            m_vertices.emplace_back(
                vertex { .position = { x0, y1 }, .color = c_border });
            m_vertices.emplace_back(
                vertex { .position = { x0, y1 }, .color = c_border });
            m_vertices.emplace_back(
                vertex { .position = { x1, y0 }, .color = c_border });
            m_vertices.emplace_back(
                vertex { .position = { x1, y1 }, .color = c_border });

            m_vertices.emplace_back(
                vertex { .position = { x2, y2 }, .color = c_tile });
            m_vertices.emplace_back(
                vertex { .position = { x3, y2 }, .color = c_tile });
            m_vertices.emplace_back(
                vertex { .position = { x2, y3 }, .color = c_tile });
            m_vertices.emplace_back(
                vertex { .position = { x2, y3 }, .color = c_tile });
            m_vertices.emplace_back(
                vertex { .position = { x3, y2 }, .color = c_tile });
            m_vertices.emplace_back(
                vertex { .position = { x3, y3 }, .color = c_tile });
          }
        }
      }

      cont->render(m_vertices);

      m_vertices.clear();
    }
  }
}
