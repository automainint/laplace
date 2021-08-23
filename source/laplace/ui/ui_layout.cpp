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

#include "../core/options.h"

namespace laplace::ui {
  auto column_layout(sl::whole width, sl::whole line_height,
                     sl::whole spacing) -> layout {

    return [width, line_height,
            spacing](rect bounds, vlayout_context context) -> vrect {
      const auto height = line_height * context.size();

      const auto elem_width  = width - spacing * 2;
      const auto elem_height = line_height - spacing * 2;

      const auto x0 = (bounds.width - width) / 2;
      const auto y0 = 0;

      auto rects = vrect {};
      rects.reserve(context.size());

      for (sl::index i = 0; i < context.size(); i++) {
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
