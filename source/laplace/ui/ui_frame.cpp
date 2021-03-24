/*  laplace/ui/ui_frame.cpp
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

#include "../graphics/utils.h"
#include "frame.h"

namespace laplace::ui {
  using graphics::viewport, graphics::clear_color_buffer;

  frame::frame() {
    m_context = context::get_default();
  }

  void frame::set_context(ptr_context cont) {
    m_context = cont;
  }

  void frame::render() {
    if (m_context) {
      rect r = {
        .x      = 0,
        .y      = 0,
        .width  = static_cast<int>(m_context->get_frame_width()),
        .height = static_cast<int>(m_context->get_frame_height())
      };

      if (is_widget_changed()) {
        m_buffer.set_size(static_cast<size_t>(r.width),
                          static_cast<size_t>(r.height));
      }

      if (is_widget_changed() || has_childs_expired()) {
        m_buffer.render([this]() {
          clear_color_buffer({ 0.f, 0.f, 0.f, 0.f });

          widget_render();
        });

        viewport(0, 0, static_cast<size_t>(r.width),
                 static_cast<size_t>(r.height));
      }

      m_context->render(r, m_buffer.color_texture);

      up_to_date();
    }
  }
}
