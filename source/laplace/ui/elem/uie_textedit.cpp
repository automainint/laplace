/*  laplace/ui/elem/uie_textedit.cpp
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

#include "textedit.h"

#include "../../core/utf8.h"
#include <utility>

namespace laplace::ui::elem {
  using namespace core::keys;

  using std::u8string_view, std::u8string, core::cref_input_handler,
      core::is_key_down, core::input_event;

  textedit::textedit() noexcept {
    set_handler(true);
  }

  void textedit::setup_filter(filter f) noexcept {
    m_filter = std::move(f);
  }

  void textedit::tick(sl::time           delta_msec,
                      cref_input_handler in) noexcept {
    textedit_tick(in);
  }

  void textedit::render(context const &con) noexcept {
    con.render_textedit(get_textedit_state());
    up_to_date();
  }

  void textedit::set_text(u8string_view text) noexcept {
    if (m_text != text) {
      m_text = u8string(text);
      set_expired(true);
    }
  }

  void textedit::set_length_limit(sl::whole limit) noexcept {
    m_length_limit = limit;

    if (m_text.size() > limit) {
      m_text.resize(limit);
      set_expired(true);
    }
  }

  void textedit::set_cursor(sl::index cursor) noexcept {
    if (m_cursor != cursor) {
      m_cursor = cursor;
      set_expired(true);
    }
  }

  void textedit::set_selection(sl::index selection) noexcept {
    if (m_selection != selection) {
      m_selection = selection;
      set_expired(true);
    }
  }

  auto textedit::get_text() const noexcept -> u8string_view {
    return m_text;
  }

  auto textedit::get_cursor() const noexcept -> sl::index {
    return m_cursor;
  }

  auto textedit::get_selection() const noexcept -> sl::index {
    return m_selection;
  }

  auto textedit::get_textedit_state() const noexcept
      -> textedit_state {
    return {
      panel::get_panel_state(), has_focus(),  get_text(),
      m_length_limit,           get_cursor(), get_selection()
    };
  }

  auto textedit::update(ptr_widget const  &object,
                        textedit_state     state,
                        filter const      &f,
                        input_event const &ev) noexcept
      -> update_result {

    auto has_focus = state.has_focus;

    auto text      = u8string { state.text };
    auto cursor    = state.cursor;
    auto selection = state.selection;

    const auto x = ev.cursor_x;
    const auto y = ev.cursor_y;

    const auto has_cursor = contains(state.bounds, x, y) &&
                            (!object || object->event_allowed(x, y));

    if (has_focus) {
      if (ev.character == ctrl_backspace) {
        auto n   = sl::index {};
        auto buf = char32_t {};

        for (sl::index i = 0; i < cursor;) {
          n = i;

          if (!utf8::decode(text, i, buf))
            break;
        }

        text.erase(text.begin() + n, text.begin() + cursor);
        cursor = n;

      } else if (ev.character != U'\0' &&
                 ev.character != ctrl_delete) {
        if (!f || f(ev.character)) {
          if (state.length_limit == 0 ||
              text.size() < state.length_limit)
            if (!utf8::encode(ev.character, text, cursor))
              error_("Unable to encode UTF-8 string.", __FUNCTION__);
        }
      }

    } else if (has_cursor && ev.key == key_lbutton &&
               is_key_down(ev)) {
      has_focus = true;
    }

    return { has_focus, text, cursor, selection };
  }

  void textedit::textedit_tick(cref_input_handler in) noexcept {
    for (auto const &ev : in.get_events()) {
      if (ev.key == key_tab && is_key_down(ev)) {
        if (auto p = get_parent(); p) {
          p->next_tab();
        }
      } else {
        auto const s = update(shared_from_this(),
                              get_textedit_state(), m_filter, ev);

        set_focus(s.has_focus);
        set_text(s.text);
        set_cursor(s.cursor);
        set_selection(s.selection);
      }
    }
  }
}
