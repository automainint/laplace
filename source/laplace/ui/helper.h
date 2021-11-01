/*  laplace/ui/helper.h
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

#ifndef laplace_ui_helper_h
#define laplace_ui_helper_h

#include "elem/textarea.h"
#include "elem/textbutton.h"
#include "elem/textedit.h"

namespace laplace::ui {
  struct helper {
    template <typename widget_>
    static inline auto make() {
      return std::make_shared<widget_>();
    }

    using widget     = ui::basic_widget;
    using textbutton = elem::textbutton;
    using textarea   = elem::textarea;
    using textedit   = elem::textedit;

    using ptr_widget     = ui::ptr_widget;
    using ptr_textbutton = elem::ptr_textbutton;
    using ptr_textarea   = elem::ptr_textarea;
    using ptr_textedit   = elem::ptr_textedit;
  };
}

#endif
