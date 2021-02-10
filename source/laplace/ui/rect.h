/*  laplace/ui/rect.h
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

#ifndef __laplace__ui_rect__
#define __laplace__ui_rect__

#include <cstdint>

namespace laplace::ui {
  struct rect {
    int x      = 0;
    int y      = 0;
    int width  = 0;
    int height = 0;
  };

  struct rectf {
    float x      = 0.f;
    float y      = 0.f;
    float width  = 0.f;
    float height = 0.f;
  };

  using cref_rect  = const rect &;
  using cref_rectf = const rectf &;

  auto compare(cref_rect a, cref_rect b) -> bool;
  auto contains(cref_rect a, int x, int y) -> bool;
  auto contains(cref_rect a, cref_rect b) -> bool;
  auto intersects(cref_rect a, cref_rect b) -> bool;

  auto to_rectf(cref_rect a) -> rectf;
  auto to_rect(cref_rectf a) -> rect;

  inline auto operator==(cref_rect a, cref_rect b) -> bool {
    return compare(a, b);
  }

  inline auto operator!=(cref_rect a, cref_rect b) -> bool {
    return !compare(a, b);
  }
}

#endif
