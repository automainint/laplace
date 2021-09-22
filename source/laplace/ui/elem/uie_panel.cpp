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
  void panel::render() {
    do {
      auto con = get_context();

      if constexpr (!_unsafe) {
        if (!con) {
          error_("No context.", __FUNCTION__);
          break;
        }
      }

      con->render_panel(get_state());
    } while (0);

    draw_childs();
    up_to_date();
  }

  auto panel::get_state() const -> panel_state {
    return { get_level(), get_absolute_rect(), is_enabled() };
  }
}
