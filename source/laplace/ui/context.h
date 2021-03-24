/*  laplace/ui/context.h
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

#ifndef laplace_ui_context_h
#define laplace_ui_context_h

#include "../graphics/texture.h"
#include "../render/context.h"
#include "elem/button.h"
#include "elem/panel.h"
#include "elem/textedit.h"
#include "text/renderer.h"

namespace laplace::ui {
  class context {
  public:
    static constexpr graphics::vec4 default_colors[] = {
      { .15f, .1f, .2f, 1.f },   { .25f, .2f, .3f, 1.f },
      { .45f, .4f, .5f, 1.f },   { .85f, .8f, .9f, 1.f },
      { .15f, .15f, .15f, 1.f }, { .25f, .25f, .25f, 1.f },
      { .45f, .45f, .45f, 1.f }, { .85f, .85f, .85f, 1.f },
    };

    context(text::ptr_renderer font = text::renderer::get_default());

    void set_frame_size(size_t width, size_t height);
    void set_colors(const graphics::vec4 colors[8]);
    void set_font(text::ptr_renderer font);

    void render(cref_rect r, graphics::ref_texture tex);

    void render(elem::panel::state panel_state);
    void render(elem::button::state button_state);
    void render(elem::button::state button_state,
                std::u8string_view  text);
    void render(elem::textedit::state textedit_state);

    auto get_frame_width() const -> size_t;
    auto get_frame_height() const -> size_t;

    static auto get_default() -> ptr_context;

  private:
    void render_solid(cref_rectf r, const graphics::vec4 color);

    using solid_vertex  = render::context::solid_vertex;
    using sprite_vertex = render::context::sprite_vertex;

    static std::weak_ptr<context> m_default;

    size_t m_frame_width  = 0;
    size_t m_frame_height = 0;

    graphics::vec4     m_colors[8];
    text::ptr_renderer m_font;

    render::ptr_context m_render = render::context::get_default();
  };
}

#endif
