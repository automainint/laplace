/*  laplace/stem/ui/su_frame.cpp
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

#include "../../graphics/utils.h"
#include "frame.h"

namespace laplace::stem::ui {
  using sprite_vertex = render::context::sprite_vertex;
  using graphics::viewport, graphics::clear_color_buffer;

  void frame::set_render_context(render::ptr_context con) {
    m_render = con;
  }

  void frame::render() {
    do {
      auto con = get_context();

      if constexpr (!_unsafe) {
        if (!m_render) {
          error_("No render context.", __FUNCTION__);
          break;
        }
      }

      const auto r = rect { .x      = 0,
                            .y      = 0,
                            .width  = con->frame_width,
                            .height = con->frame_height };

      if (is_widget_changed()) {
        m_buffer.set_size(r.width, r.height);
      }

      if (is_widget_changed() || has_childs_expired()) {
        m_buffer.render([this]() {
          clear_color_buffer({ 0.f, 0.f, 0.f, 0.f });

          widget_render();
        });

        viewport(0, 0, r.width, r.height);
      }

      const auto f = to_rectf(r);

      const auto v = {
        sprite_vertex { .position = { f.x, f.y },
                        .texcoord = { 0.f, 1.f } },
        sprite_vertex { .position = { f.x + f.width, f.y },
                        .texcoord = { 1.f, 1.f } },
        sprite_vertex { .position = { f.x, f.y + f.height },
                        .texcoord = { 0.f, 0.f } },
        sprite_vertex { .position = { f.x + f.width, f.y + f.height },
                        .texcoord = { 1.f, 0.f } }
      };

      m_render->render_strip(v, m_buffer.color_texture);
    } while (0);

    up_to_date();
  }
}
