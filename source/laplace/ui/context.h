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

#include "../core/defs.h"
#include "rect.h"
#include <functional>

namespace laplace::ui {
  struct text_area {
    sl::index top    = 0;
    sl::index width  = 0;
    sl::index height = 0;
  };

  struct panel_state {
    sl::index level      = 0;
    rect      rect       = {};
    bool      is_enabled = true;
  };

  struct button_state : panel_state {
    bool is_pressed = false;
    bool has_cursor = false;
  };

  struct textbutton_state : button_state {
    std::u8string_view text;
  };

  struct textedit_state : panel_state {
    bool               has_focus = false;
    std::u8string_view text;
    sl::whole          length_limit = 0;
    sl::index          cursor       = 0;
    sl::index          selection    = 0;
  };

  using fn_prepare     = std::function<void()>;
  using fn_adjust_text = std::function<text_area(std::u8string_view)>;
  using fn_render_text =
      std::function<void(rect, std::u8string_view)>;
  using fn_render_panel      = std::function<void(panel_state)>;
  using fn_render_button     = std::function<void(button_state)>;
  using fn_render_textbutton = std::function<void(textbutton_state)>;
  using fn_render_textedit   = std::function<void(textedit_state)>;

  /*  UI context interface.
   */
  struct context {
    sl::whole frame_width  = 0;
    sl::whole frame_height = 0;

    fn_prepare           prepare;
    fn_adjust_text       adjust_text;
    fn_render_text       render_text;
    fn_render_panel      render_panel;
    fn_render_button     render_button;
    fn_render_textbutton render_textbutton;
    fn_render_textedit   render_textedit;
  };

  using ptr_context = std::shared_ptr<context>;
}

#endif
