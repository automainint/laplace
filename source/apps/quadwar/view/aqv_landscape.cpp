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

#include "landscape.h"

#include "../../../laplace/render/context.h"
#include "../object/landscape.h"
#include "../object/pathmap.h"
#include "../object/root.h"

namespace quadwar_app::view {
  using std::span, graphics::flat::solid_shader;

  const real landscape::tile_border = .2f;

  void landscape::render(const camera &cam, world w) {
    const auto r    = w.get_entity(w.get_root());
    const auto land = w.get_entity(object::root::get_landscape(r));

    const auto ver = object::landscape::get_version(land);

    if (m_state_version != ver) {

      const auto width  = object::landscape::get_width(land);
      const auto height = object::landscape::get_height(land);
      const auto tiles  = object::landscape::get_tiles(land);

      if (tiles.size() != width * height)
        return;

      const auto c_tile   = vec4 { .3f, .25f, .2f, 1.f };
      const auto c_border = vec4 { .2f, .15f, .15f, .7f };

      const auto fw = static_cast<real>(width);
      const auto fh = static_cast<real>(height);

      const auto tile_size   = cam.get_grid_scale();
      const auto border_size = tile_size * tile_border;

      const auto bias = .5f / object::pathmap::resolution;

      m_vertices.reserve(12 * width * height);
      m_vertices.clear();

      for (sl::index j = 0; j < height; j++) {
        for (sl::index i = 0; i < width; i++) {

          if (tiles[j * width + i] != object::landscape::tile_walkable) {

            const auto x0 = (static_cast<real>(i) - bias) * tile_size;
            const auto y0 = (static_cast<real>(j) - bias) * tile_size;

            const auto x1 = x0 + tile_size;
            const auto y1 = y0 + tile_size;

            const auto x2 = x0 + border_size;
            const auto y2 = y0 + border_size;

            const auto x3 = x1 - border_size;
            const auto y3 = y1 - border_size;

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

      m_state_version = ver;
      m_width         = width;
      m_height        = height;
    }

    auto cont = render::context::get_default();

    if (cont) {
      const auto [p, s] = cam.get_transform();
      cont->render(m_vertices, p, s);
    }
  }
}
