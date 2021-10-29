/*  laplace/stem/ui/context.h
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

#ifndef laplace_stem_ui_context_h
#define laplace_stem_ui_context_h

#include "../../graphics/texture.h"
#include "../../render/context.h"
#include "../../ui/context.h"
#include "../text/renderer.h"

namespace laplace::stem::ui {
  using namespace laplace::ui;

  /*  UI context implementation.
   */
  class context_impl : public context {
  public:
    static const graphics::vec4 default_colors[];

    graphics::vec4 colors[8];

    context_impl(
        text::ptr_renderer font = text::renderer::get_default());

    void set_font(text::ptr_renderer font);

  private:
    void _render_panel(panel_state state);
    void _render_button(button_state state);
    void _render_textbutton(textbutton_state state);
    void _render_textedit(textedit_state state);

    void _render_solid(rectf r, const graphics::vec4 color);

    using solid_vertex = render::context::solid_vertex;

    text::ptr_renderer  m_font;
    render::ptr_context m_render = render::context::get_default();
  };
}

#endif
