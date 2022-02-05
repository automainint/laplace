/*  Copyright (c) 2022 Mitya Selivanov
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

    void setup(graphics::flat::ptr_solid_shader shader) noexcept;
    void setup(graphics::flat::ptr_sprite_shader shader) noexcept;

    void adjust_frame_size(sl::whole width,
                           sl::whole height) noexcept;

    void render(std::span<solid_vertex const> vertices) noexcept;

    void render(std::span<solid_vertex const> vertices,
                graphics::cref_vec2           position,
                graphics::cref_vec2           scale) noexcept;

    void render_strip(
        std::span<solid_vertex const> vertices) noexcept;

    void render(std::span<sprite_vertex const> vertices,
                graphics::ref_texture          tex) noexcept;

    void render(std::span<sprite_vertex const> vertices,
                graphics::cref_vec2            position,
                graphics::cref_vec2            scale,
                graphics::ref_texture          tex) noexcept;

    void render_strip(std::span<sprite_vertex const> vertices,
                      graphics::ref_texture          tex) noexcept;

    [[nodiscard]] static auto get_default() noexcept -> ptr_context;

  private:
    static std::weak_ptr<context> m_default;

    graphics::flat::solid_buffer  m_solid_buffer;
    graphics::flat::sprite_buffer m_sprite_buffer;

    graphics::flat::ptr_solid_shader  m_solid_shader;
    graphics::flat::ptr_sprite_shader m_sprite_shader;
  };

  using ref_context  = context &;
  using cref_context = context const &;
  using ptr_context  = context::ptr_context;
}

#endif
