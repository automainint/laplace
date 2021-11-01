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

#include "panel.h"

#include "../context.h"

namespace laplace::ui::elem {
  void panel::render(context const &con) noexcept {
    con.render_panel(get_panel_state());
    draw_childs(con);
    up_to_date();
  }

  auto panel::get_panel_state() const noexcept -> panel_state {
    return { get_level(), get_absolute_rect(), is_enabled() };
  }
}
