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
    sl::index level = 0;
    rect      box;
  };

  using layout = std::function<sl::vector<rect>(
      rect bounds, std::span<layout_context const> context)>;

  auto column_layout(sl::whole width,
                     sl::whole line_height,
                     sl::whole spacing) noexcept -> layout;

  auto column_layout(std::tuple<sl::whole, sl::whole> widths,
                     sl::whole                        line_height,
                     sl::whole spacing) noexcept -> layout;
}

#endif
