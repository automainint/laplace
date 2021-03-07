/*  apps/quadwar/view/game.h
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

#ifndef quadwar_view_game_h
#define quadwar_view_game_h

#include "camera.h"
#include "landscape.h"

namespace quadwar_app::view {
  class game : engine::helper {
  public:
    void adjust_layout(int width, int height);

    void move(const vec2 delta);
    void scale(const real delta);

    void render(engine::access::world w);

  private:
    [[nodiscard]] auto get_scale() const -> real;

    void update_bounds(world w);

    real      m_scale_ln     = 0.f;
    camera    m_camera;
    landscape m_landscape;
  };
}

#endif
