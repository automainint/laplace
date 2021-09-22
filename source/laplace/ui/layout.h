/*  laplace/ui/layout.h
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

#ifndef laplace_ui_layout_h
#define laplace_ui_layout_h

#include "rect.h"
#include <functional>

namespace laplace::ui {
  struct layout_context {
    sl::index level;
    rect      box;
  };

  using vrect           = sl::vector<rect>;
  using vlayout_context = sl::vector<layout_context>;

  using layout =
      std::function<vrect(rect bounds, vlayout_context context)>;

  auto column_layout(sl::whole width, sl::whole line_height,
                     sl::whole spacing) -> layout;
}

#endif
