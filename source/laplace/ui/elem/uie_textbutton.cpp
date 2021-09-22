/*  laplace/ui/elem/uei_textbutton.cpp
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

#include "textbutton.h"

namespace laplace::ui::elem {
  using std::u8string_view;

  void textbutton::render() {
    do {
      auto con = get_context();

      if constexpr (!_unsafe) {
        if (!con) {
          error_("No context.", __FUNCTION__);
          break;
        }
      }

      con->render_textbutton(get_state());
    } while (0);

    up_to_date();
  }

  void textbutton::set_text(u8string_view text) {
    m_text = text;
  }

  auto textbutton::get_state() const -> textbutton_state {
    return { button::get_state(), m_text };
  }
}
