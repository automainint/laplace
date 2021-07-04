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
    static constexpr auto default_color = vec4 { .8f, .1f, .1f, 1.f };
    static constexpr auto highlight_color = vec4 { .8f, .6f, .6f, 1.f };
    static constexpr auto selection_color = vec4 { .8f, .4f, .4f, 1.f };
    static constexpr auto unit_scaling    = .9f;

    void render(const camera &cam, world w,
                std::span<const sl::index> highlight,
                std::span<const sl::index> selection);

    struct unit_info {
      sl::index id;
      vec2      rect[2];
    };

    [[nodiscard]] auto get_units() const -> std::span<const unit_info>;

  private:
    void update_units(const camera &cam, world w);

    void update_buffer(std::span<const sl::index> highlight,
                       std::span<const sl::index> selection);

    using vertex = graphics::flat::solid_shader::vertex;

    sl::vector<vertex>    m_vertices;
    sl::vector<unit_info> m_info;
  };
}

#endif
