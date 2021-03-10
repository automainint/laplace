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
#include "../graphics/tridi/mesh_shader.h"
#include "../graphics/tridi/reflect_shader.h"
#include "../graphics/tridi/refract_shader.h"
#include "../graphics/tridi/shadow_shader.h"
#include "../graphics/tridi/uvmap_shader.h"
#include <array>
#include <memory>
#include <string>

namespace laplace::render {
  enum class mode : uint8_t { disabled, low, high, ultra };

  struct settings {
    mode blending   = mode::disabled;
    mode lighting   = mode::disabled;
    mode shadows    = mode::disabled;
    mode volumes    = mode::disabled;
    mode reflection = mode::disabled;
    mode refraction = mode::disabled;
  };

  using cref_settings = const settings &;

  class context {
  public:
    using solid_vertex  = graphics::flat::solid_shader::vertex;
    using sprite_vertex = graphics::flat::sprite_shader::vertex;

    using ptr_context = std::shared_ptr<context>;

    static constexpr auto default_settings = settings {};

    void setup(graphics::flat::ptr_solid_shader shader);
    void setup(graphics::flat::ptr_sprite_shader shader);
    void setup(graphics::tridi::ptr_mesh_shader shader);
    void setup(graphics::tridi::ptr_uvmap_shader shader);
    void setup(graphics::tridi::ptr_shadow_shader shader);
    void setup(graphics::tridi::ptr_reflect_shader shader);
    void setup(graphics::tridi::ptr_refract_shader shader);

    void set_settings(cref_settings sets);

    void render(std::span<const solid_vertex> vertices);
    void render_strip(std::span<const solid_vertex> vertices);

    void render(std::span<const sprite_vertex> vertices,
                graphics::ref_texture          tex);

    void render_strip(std::span<const sprite_vertex> vertices,
                      graphics::ref_texture          tex);

    static auto get_default() -> ptr_context;

  private:
    static std::weak_ptr<context> m_default;

    settings   m_settings = default_settings;

    graphics::flat::solid_buffer  m_solid_buffer;
    graphics::flat::sprite_buffer m_sprite_buffer;

    graphics::flat::ptr_solid_shader    m_solid_shader;
    graphics::flat::ptr_sprite_shader   m_sprite_shader;
    graphics::tridi::ptr_mesh_shader    m_mesh_shader;
    graphics::tridi::ptr_uvmap_shader   m_uvmap_shader;
    graphics::tridi::ptr_shadow_shader  m_shadow_shader;
    graphics::tridi::ptr_reflect_shader m_reflect_shader;
    graphics::tridi::ptr_refract_shader m_refract_shader;
  };

  using ref_context  = context &;
  using cref_context = const context &;
  using ptr_context  = context::ptr_context;
}

#endif
