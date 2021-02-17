/*  laplace/ui/elem/uie_panel.cpp
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

#include "../context.h"
#include "panel.h"

namespace laplace::ui::elem {
  void panel::set_context(ptr_context cont) {
    m_context = cont ? cont : ui::context::get_default();
    set_expired(true);
  }

  void panel::render() {
    m_context->render(get_state());

    draw_childs();

    up_to_date();
  }

  auto panel::get_state() const -> panel::state {
    return { get_level(), get_absolute_rect(), is_enabled() };
  }
}
