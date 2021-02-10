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

#ifndef __laplace__ui_layout__
#define __laplace__ui_layout__

#include "rect.h"
#include <functional>
#include <vector>

namespace laplace::ui {
  struct layout_context {
    size_t    level;
    cref_rect box;
  };

  using vrect           = std::vector<rect>;
  using vlayout_context = std::vector<layout_context>;

  using layout = std::function<vrect( //
      cref_rect       bounds,         //
      vlayout_context context)>;

  auto column_layout(int width, int line_height, int spacing)
      -> layout;
}

#endif
