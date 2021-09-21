/*  laplace/ui/elem/slider.h
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

#ifndef laplace_ui_elem_slider_h
#define laplace_ui_elem_slider_h

#include "../widget.h"

namespace laplace::ui::elem {
  class slider : public widget {
  public:
    struct state {
      sl::index level;
      rect   slider_rect;
      int    begin;
      int    end;
      int    value;
    };
  };
}

#endif
