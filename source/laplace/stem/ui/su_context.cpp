/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../core/utf8.h"
#include "../../graphics/utils.h"
#include "context.h"

#include <utility>

namespace laplace::stem::ui {
  using std::make_shared, std::span, std::weak_ptr,
      std::u8string_view, graphics::ref_texture, graphics::vec4,
      text::ptr_renderer, laplace::ui::panel_state,
      laplace::ui::button_state, laplace::ui::textedit_state,
      laplace::ui::text_area, laplace::ui::rect,
      laplace::ui::textbutton_state, laplace::ui::rectf;

  vec4 const context_impl::default_colors[] = {
    { .15f, .1f, .2f, 1.f },   { .25f, .2f, .3f, 1.f },
    { .45f, .4f, .5f, 1.f },   { .85f, .8f, .9f, 1.f },
    { .15f, .15f, .15f, 1.f }, { .25f, .25f, .25f, 1.f },
    { .45f, .45f, .45f, 1.f }, { .85f, .85f, .85f, 1.f },
  };

  context_impl::context_impl(text::ptr_renderer font) noexcept :
      m_font(std::move(font)) {
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

    render_text = [this](rect r, std::u8string_view text) noexcept {
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
  }

  void context_impl::set_font(text::ptr_renderer font) noexcept {
    m_font = std::move(font);
  }

  void context_impl::_render_panel(panel_state state) noexcept {
    if constexpr (!_unsafe) {
      if (!m_render) {
        error_("No render context.", __FUNCTION__);
        return;
      }
    }

    _render_solid(to_rectf(state.bounds), colors[0]);
  }

  void context_impl::_render_button(button_state state) noexcept {
    if constexpr (!_unsafe) {
      if (!m_render) {
        error_("No render context.", __FUNCTION__);
        return;
      }
    }

    sl::whole const n0 = state.is_enabled ? 0 : 4;

    auto n_color = sl::whole {};

    if (state.is_pressed) {
      n_color = n0 + 2;
    } else if (state.has_cursor) {
      n_color = n0 + 1;
    } else {
      n_color = n0;
    }

    _render_solid(to_rectf(state.bounds), colors[n_color]);
  }

  void context_impl::_render_textbutton(
      textbutton_state state) noexcept {
    if constexpr (!_unsafe) {
      if (!m_font) {
        error_("No text renderer.", __FUNCTION__);
        return;
      }
    }

    this->_render_button(state);

    auto a = m_font->adjust(state.text);

    auto dx = state.bounds.width - a.width;
    auto dy = state.bounds.height - a.height;

    auto x = state.bounds.x + dx / 2;
    auto y = state.bounds.y + dy / 2;

    m_font->render(x, y, state.text);
  }

  void context_impl::_render_textedit(textedit_state state) noexcept {
    if constexpr (!_unsafe) {
      if (!m_font) {
        error_("No text renderer.", __FUNCTION__);
        return;
      }
    }

    auto const f = to_rectf(state.bounds);

    if (state.has_focus) {
      _render_solid(f, colors[1]);
    } else {
      _render_solid(f, colors[0]);
    }

    auto const text_rect = m_font->adjust(state.text);
    auto const dy        = state.bounds.height - text_rect.height;
    auto const x         = state.bounds.x + dy / 2;
    auto const y         = state.bounds.y + dy / 2;

    m_font->render(x, y, state.text);

    if (state.has_focus) {
      if (state.flash < 0) {
        auto const cursor_rect = m_font->adjust(
            { state.text.begin(),
              state.text.begin() +
                  utf8::offset(state.text, state.cursor) });

        auto const flash_rect = m_font->adjust(u8"|");

        auto const flash_x = x + cursor_rect.width -
                             flash_rect.width / 2;
        auto const flash_y = y;

        m_font->render(flash_x, flash_y, u8"|");
      }
    }
  }

  void context_impl::_render_solid(rectf      r,
                                   vec4 const color) noexcept {
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
