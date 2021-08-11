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

#include "../context.h"

namespace laplace::ui::elem {
  using std::u8string_view;

  void textbutton::render() {
    if (!m_context) {
      error_("No context.", __FUNCTION__);
      return;
    }

    m_context->render(get_state(), m_text);

    up_to_date();
  }

  void textbutton::set_text(u8string_view text) {
    m_text = text;
  }
}
