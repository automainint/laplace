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
    using vertex = solid_shader::vertex;

    auto cont = render::context::get_default();

    if (cont) {
      const auto r    = w.get_entity(w.get_root());
      const auto land = w.get_entity(object::root::get_landscape(r));

      const auto width  = object::landscape::get_width(land);
      const auto height = object::landscape::get_height(land);

      const auto color = vec4 { .3f, .25f, .2f, 1.f };

      const auto fx = cam.get_frame().x() / 2.f;
      const auto fy = cam.get_frame().y() / 2.f;

      const auto px = -cam.get_position().x();
      const auto py = -cam.get_position().y();

      const auto s = cam.get_scale();
      const auto d = tail_size - tail_border * 2.f;

      for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {

          if (object::landscape::get_tile(land, i, j)) {
            const auto x0 = fx + (px + tail_border + i * tail_size) * s;
            const auto y0 = fy + (py + tail_border + j * tail_size) * s;
            const auto x1 = x0 + d * s;
            const auto y1 = y0 + d * s;

            const auto v = {
              vertex { .position = { x0, y0 }, .color = color },
              vertex { .position = { x1, y0 }, .color = color },
              vertex { .position = { x0, y1 }, .color = color },
              vertex { .position = { x1, y1 }, .color = color }
            };

            cont->render_strip(v);
          }
        }
      }
    }
  }
}
