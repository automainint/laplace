/*  laplace/render/context.h
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

#ifndef laplace_render_context_h
#define laplace_render_context_h

#include "../graphics/flat/solid_buffer.h"
#include "../graphics/flat/solid_shader.h"
#include "../graphics/flat/sprite_buffer.h"
#include "../graphics/flat/sprite_shader.h"
#include "../graphics/texture.h"

namespace laplace::render {
  class context {
  public:
    using solid_vertex  = graphics::flat::solid_shader::vertex;
    using sprite_vertex = graphics::flat::sprite_shader::vertex;

    using ptr_context = std::shared_ptr<context>;

    void setup(graphics::flat::ptr_solid_shader shader);
    void setup(graphics::flat::ptr_sprite_shader shader);

    void adjust_frame_size(sl::whole width, sl::whole height);

    void render(std::span<const solid_vertex> vertices);

    void render(std::span<const solid_vertex> vertices,
                graphics::cref_vec2           position,
                graphics::cref_vec2           scale);

    void render_strip(std::span<const solid_vertex> vertices);

    void render(std::span<const sprite_vertex> vertices,
                graphics::ref_texture          tex);

    void render(std::span<const sprite_vertex> vertices,
                graphics::cref_vec2            position,
                graphics::cref_vec2 scale, graphics::ref_texture tex);

    void render_strip(std::span<const sprite_vertex> vertices,
                      graphics::ref_texture          tex);

    static auto get_default() -> ptr_context;

  private:
    static std::weak_ptr<context> m_default;

    graphics::flat::solid_buffer  m_solid_buffer;
    graphics::flat::sprite_buffer m_sprite_buffer;

    graphics::flat::ptr_solid_shader    m_solid_shader;
    graphics::flat::ptr_sprite_shader   m_sprite_shader;
  };

  using ref_context  = context &;
  using cref_context = const context &;
  using ptr_context  = context::ptr_context;
}

#endif
