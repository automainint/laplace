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

#include "frame.h"

#include "../../graphics/utils.h"
#include <utility>

namespace laplace::stem::ui {
  using sprite_vertex = render::context::sprite_vertex;
  using graphics::viewport, graphics::clear_color_buffer,
      laplace::ui::rect, laplace::ui::basic_widget,
      laplace::ui::context;

  void frame::set_render_context(render::ptr_context con) noexcept {
    m_render = std::move(con);
  }

  void frame::render(context const &con) noexcept {
    do {
      if constexpr (!_unsafe) {
        if (!m_render) {
          error_("No render context.", __FUNCTION__);
          break;
        }
      }

      auto const r = rect { .x      = 0,
                            .y      = 0,
                            .width  = con.frame_width,
                            .height = con.frame_height };

      if (is_widget_changed()) {
        m_buffer.set_size(r.width, r.height);
      }

      if (is_widget_changed() || has_attached_expired()) {
        m_buffer.render([&]() {
          clear_color_buffer({ 0.f, 0.f, 0.f, 0.f });
          widget_render(con);
        });

        viewport(0, 0, r.width, r.height);
      }

      auto const f = to_rectf(r);

      auto const v = {
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
    } while (false);

    up_to_date();
  }
}
