/*  laplace/ui/elem/sprite.h
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

#ifndef laplace_ui_elem_sprite_h
#define laplace_ui_elem_sprite_h

#include "../../graphics/texture.h"
#include "panel.h"

namespace laplace::ui::elem {
  class sprite : public panel {
  public:
    struct state : panel::state {
      graphics::ptr_texture texture;
    };
  };
}

#endif
