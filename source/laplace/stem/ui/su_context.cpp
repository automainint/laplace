/*  laplace/ui/ui_context.cpp
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

#include "../../graphics/utils.h"
#include "context.h"

namespace laplace::stem::ui {
  using std::make_shared, std::span, std::weak_ptr,
      std::u8string_view, graphics::ref_texture, graphics::vec4,
      text::ptr_renderer, ui::panel_state, ui::button_state,
      ui::textedit_state;

  vec4 const context_impl::default_colors[] = {
    { .15f, .1f, .2f, 1.f },   { .25f, .2f, .3f, 1.f },
    { .45f, .4f, .5f, 1.f },   { .85f, .8f, .9f, 1.f },
    { .15f, .15f, .15f, 1.f }, { .25f, .25f, .25f, 1.f },
    { .45f, .45f, .45f, 1.f }, { .85f, .85f, .85f, 1.f },
  };

  context_impl::context_impl(text::ptr_renderer font) {
    prepare = []() { graphics::prepare_ui(); };

    adjust_text = [this](u8string_view text) -> text_area {
      if constexpr (!_unsafe) {
        if (!m_font) {
          error_("No text renderer.", __FUNCTION__);
          return {};
        }
      }

      return m_font->adjust(text);
    };

    render_text = [this](rect r, std::u8string_view text) {
      if constexpr (!_unsafe) {
        if (!m_font) {
          error_("No text renderer.", __FUNCTION__);
          return;
        }
      }

      m_font->render(r.x, r.y, text);
    };

    render_panel = [this](panel_state state) {
      this->_render_panel(state);
    };

    render_button = [this](button_state state) {
      this->_render_button(state);
    };

    render_textbutton = [this](textbutton_state state) {
      this->_render_textbutton(state);
    };

    render_textedit = [this](textedit_state state) {
      this->_render_textedit(state);
    };

    constexpr auto colors_count = sizeof colors / sizeof *colors;

    std::copy(default_colors, default_colors + colors_count, colors);

    m_font = font;
  }

  void context_impl::set_font(text::ptr_renderer font) {
    m_font = font;
  }

  void context_impl::_render_panel(panel_state state) {
    if constexpr (!_unsafe) {
      if (!m_render) {
        error_("No render context.", __FUNCTION__);
        return;
      }
    }

    _render_solid(to_rectf(state.rect), colors[0]);
  }

  void context_impl::_render_button(button_state state) {
    if constexpr (!_unsafe) {
      if (!m_render) {
        error_("No render context.", __FUNCTION__);
        return;
      }
    }

    const sl::whole n0 = state.is_enabled ? 0 : 4;

    sl::whole n_color = 0;

    if (state.is_pressed) {
      n_color = n0 + 2;
    } else if (state.has_cursor) {
      n_color = n0 + 1;
    } else {
      n_color = n0;
    }

    _render_solid(to_rectf(state.rect), colors[n_color]);
  }

  void context_impl::_render_textbutton(textbutton_state state) {
    if constexpr (!_unsafe) {
      if (!m_font) {
        error_("No text renderer.", __FUNCTION__);
        return;
      }
    }

    this->_render_button(state);

    auto a = m_font->adjust(state.text);

    auto dx = state.rect.width - a.width;
    auto dy = state.rect.height - a.height;

    auto x = state.rect.x + dx / 2;
    auto y = state.rect.y + dy / 2;

    m_font->render(x, y, state.text);
  }

  void context_impl::_render_textedit(textedit_state state) {
    if constexpr (!_unsafe) {
      if (!m_font) {
        error_("No text renderer.", __FUNCTION__);
        return;
      }
    }

    auto const f = to_rectf(state.rect);

    if (state.has_focus) {
      _render_solid(f, colors[1]);
    } else {
      _render_solid(f, colors[0]);
    }

    auto const a  = m_font->adjust(state.text);
    auto const dy = state.rect.height - a.height;
    auto const x  = state.rect.x + dy / 2;
    auto const y  = state.rect.y + dy / 2;

    m_font->render(x, y, state.text);
  }

  void context_impl::_render_solid(rectf r, const vec4 color) {
    if constexpr (!_unsafe) {
      if (!m_render) {
        error_("No render context.", __FUNCTION__);
        return;
      }
    }

    const auto v = {
      solid_vertex { .position = { r.x, r.y }, .color = color },
      solid_vertex { .position = { r.x + r.width, r.y },
                     .color    = color },
      solid_vertex { .position = { r.x, r.y + r.height },
                     .color    = color },
      solid_vertex { .position = { r.x + r.width, r.y + r.height },
                     .color    = color }
    };

    m_render->render_strip(v);
  }
}
