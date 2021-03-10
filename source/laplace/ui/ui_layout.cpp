/*  laplace/ui/ui_layout.cpp
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

#include "layout.h"

namespace laplace::ui {
  auto column_layout(int width, int line_height, int spacing)
      -> layout {

    return [width, line_height, spacing]( //
               rect            bounds,    //
               vlayout_context context) -> vrect {
      const auto height = line_height * context.size();

      const auto elem_width  = width - spacing * 2;
      const auto elem_height = line_height - spacing * 2;

      const auto x0 = (bounds.width - width) / 2;
      const auto y0 = 0;

      vrect rects;
      rects.reserve(context.size());

      for (size_t i = 0; i < context.size(); i++) {
        rects.emplace_back(rect {
            .x     = x0 + spacing,
            .y     = y0 + spacing + line_height * static_cast<int>(i),
            .width = elem_width,
            .height = elem_height });
      }

      return rects;
    };
  }
}
