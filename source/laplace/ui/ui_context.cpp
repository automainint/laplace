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

#include "context.h"

namespace laplace::ui {
  using std::make_shared, std::span, std::weak_ptr,
      std::u8string_view, graphics::ref_texture, graphics::vec4,
      text::ptr_renderer, elem::panel, elem::button, elem::textedit;

  weak_ptr<context> context::m_default;

  context::context(text::ptr_renderer font) {
    set_colors(default_colors);
    m_font = font;
  }

  void context::set_frame_size(size_t width, size_t height) {
    m_frame_width  = width;
    m_frame_height = height;
  }

  void context::set_colors(const vec4 colors[8]) {
    m_colors[0] = colors[0];
    m_colors[1] = colors[1];
    m_colors[2] = colors[2];
    m_colors[3] = colors[3];
    m_colors[4] = colors[4];
    m_colors[5] = colors[5];
    m_colors[6] = colors[6];
    m_colors[7] = colors[7];
  }

  void context::set_font(text::ptr_renderer font) {
    m_font = font;
  }

  void context::render(cref_rect r, ref_texture tex) {
    if (m_render) {
      const auto f = to_rectf(r);

      const auto v = {
        sprite_vertex { .position = { f.x, f.y },
                        .texcoord = { 0.f, 1.f } },
        sprite_vertex { .position = { f.x + f.width, f.y },
                        .texcoord = { 1.f, 1.f } },
        sprite_vertex { .position = { f.x, f.y + f.height },
                        .texcoord = { 0.f, 0.f } },
        sprite_vertex { .position = { f.x + f.width, f.y + f.height },
                        .texcoord = { 1.f, 0.f } }
      };

      m_render->render_strip(v, tex);
    }
  }

  void context::render(panel::state panel_state) {
    if (m_render) {
      render_solid(to_rectf(panel_state.rect), m_colors[0]);
    }
  }

  void context::render(button::state button_state) {
    if (m_render) {
      const size_t n0 = button_state.is_enabled ? 0u : 4u;

      size_t n_color = 0;

      if (button_state.is_pressed) {
        n_color = n0 + 2;
      } else if (button_state.has_cursor) {
        n_color = n0 + 1;
      } else {
        n_color = n0;
      }

      render_solid(to_rectf(button_state.rect), m_colors[n_color]);
    }
  }

  void context::render(button::state button_state, u8string_view text) {
    this->render(button_state);

    auto a = m_font->adjust(text);

    auto dx = button_state.rect.width - a.width;
    auto dy = button_state.rect.height - a.height;

    auto x = button_state.rect.x + dx / 2;
    auto y = button_state.rect.y + dy / 2;

    m_font->render(x, y, text);
  }

  void context::render(textedit::state textedit_state) {
    auto f = to_rectf(textedit_state.rect);

    if (textedit_state.has_focus) {
      render_solid(f, m_colors[1]);
    } else {
      render_solid(f, m_colors[0]);
    }

    auto a = m_font->adjust(textedit_state.text);

    int dy = textedit_state.rect.height - a.height;

    int x = textedit_state.rect.x + dy / 2;
    int y = textedit_state.rect.y + dy / 2;

    m_font->render(x, y, textedit_state.text);
  }

  auto context::get_frame_width() const -> size_t {
    return m_frame_width;
  }

  auto context::get_frame_height() const -> size_t {
    return m_frame_height;
  }

  auto context::get_default() -> ptr_context {
    auto cont = m_default.lock();

    if (!cont) {
      cont      = make_shared<context>();
      m_default = cont;
    }

    return cont;
  }

  void context::render_solid(cref_rectf r, const vec4 color) {
    if (m_render) {
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
}
