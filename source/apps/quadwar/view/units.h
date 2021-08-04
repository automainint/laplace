/*  apps/quadwar/view/units.h
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

#ifndef quadwar_view_units_h
#define quadwar_view_units_h

#include "../../../laplace/graphics/flat/solid_shader.h"
#include "camera.h"
#include "defs.h"

namespace quadwar_app::view {
  class units : engine::helper {
  public:
    static const vec4 colors[];
    static const vec4 highlight_color;
    static const vec4 selection_color;
    static const real unit_scaling;
    static const real selection_delta;

    void render(const camera &cam, world w,
                std::span<const sl::index> highlight,
                std::span<const sl::index> selection);

    struct unit_info {
      sl::index id          = {};
      sl::index color_index = {};
      vec2      rect[2];
    };

    [[nodiscard]] auto get_units() const -> std::span<const unit_info>;

  private:
    void update_units(const camera &cam, world w);

    void update_buffer(std::span<const sl::index> highlight,
                       std::span<const sl::index> selection);

    using vertex = graphics::flat::solid_shader::vertex;

    sl::vector<vertex>    m_marks;
    sl::vector<vertex>    m_units;
    sl::vector<unit_info> m_info;
  };
}

#endif
