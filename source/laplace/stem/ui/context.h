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
  /*  UI context implementation.
   */
  class context_impl : public laplace::ui::context {
  public:
    static const graphics::vec4 default_colors[];

    log_handler log = get_global_log();

    graphics::vec4 colors[8];

    explicit context_impl(
        text::ptr_renderer font =
            text::renderer::get_default(get_global_log())) noexcept;

    void set_font(text::ptr_renderer font) noexcept;

  private:
    void _render_panel(laplace::ui::panel_state state) noexcept;

    void _render_button(laplace::ui::button_state state) noexcept;

    void _render_textbutton(
        laplace::ui::textbutton_state state) noexcept;

    void _render_textedit(laplace::ui::textedit_state state) noexcept;

    void _render_solid(laplace::ui::rectf r,
                       graphics::vec4     color) noexcept;

    using solid_vertex = render::context::solid_vertex;

    text::ptr_renderer  m_font;
    render::ptr_context m_render = render::context::get_default();
  };
}

#endif
