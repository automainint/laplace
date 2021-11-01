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
  using std::span, std::tuple, std::get;

  auto column_layout(sl::whole width,
                     sl::whole line_height,
                     sl::whole spacing) noexcept -> layout {
    return
        [width, line_height, spacing](
            rect                       bounds,
            span<layout_context const> context) -> sl::vector<rect> {
          auto const elem_width  = width - spacing * 2;
          auto const elem_height = line_height - spacing * 2;

          auto const x0 = (bounds.width - width) / 2;
          auto const y0 = 0;

          auto rects = sl::vector<rect> {};
          rects.reserve(context.size());

          for (sl::index i = 0; i < context.size(); i++)
            rects.emplace_back<rect>(
                { .x      = x0 + spacing,
                  .y      = y0 + spacing + line_height * i,
                  .width  = elem_width,
                  .height = elem_height });

          return rects;
        };
  }

  auto column_layout(tuple<sl::whole, sl::whole> widths,
                     sl::whole                   line_height,
                     sl::whole spacing) noexcept -> layout {
    return
        [widths, line_height, spacing](
            rect                       bounds,
            span<layout_context const> context) -> sl::vector<rect> {
          auto const full_width  = get<0>(widths) + get<1>(widths);
          auto const col0_width  = get<0>(widths) - spacing * 2;
          auto const col1_width  = get<1>(widths) - spacing * 2;
          auto const elem_height = line_height - spacing * 2;

          auto const x0 = (bounds.width - full_width) / 2;
          auto const x1 = x0 + col0_width + spacing * 2;
          auto const y0 = 0;

          auto rects = sl::vector<rect> {};
          rects.reserve(context.size());

          for (sl::index i = 0; i < context.size(); i += 2) {
            rects.emplace_back<rect>(
                { .x      = x0 + spacing,
                  .y      = y0 + spacing + line_height * i / 2,
                  .width  = col0_width,
                  .height = elem_height });

            if (i + 1 < context.size())
              rects.emplace_back<rect>(
                  { .x      = x1 + spacing,
                    .y      = y0 + spacing + line_height * i / 2,
                    .width  = col1_width,
                    .height = elem_height });
          }

          return rects;
        };
  }
}
