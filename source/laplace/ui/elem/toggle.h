/*  laplace/ui/elem/toggle.h
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

#ifndef laplace_ui_elem_toggle_h
#define laplace_ui_elem_toggle_h

#include "../widget.h"

namespace laplace::ui::elem {
  class toggle : public widget {
  public:
    struct state {
      sl::index level;
      rect      toggle_rect;
      bool      is_pressed;
      bool      is_on;
    };
  };
}

#endif
