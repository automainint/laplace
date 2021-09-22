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

#include "../../core/keys.h"
#include "../../core/utf8.h"

namespace laplace::ui::elem {
  using namespace core::keys;

  using std::u8string_view, std::u8string, core::cref_input_handler;

  textedit::textedit() {
    set_handler(true);
  }

  void textedit::setup_filter(filter f) {
    m_filter = f;
  }

  auto textedit::tick(sl::time           delta_msec,
                      cref_input_handler in,
                      bool               is_handled) -> bool {
    return is_handled || textedit_tick(in);
  }

  void textedit::render() {
    do {
      auto con = get_context();

      if constexpr (!_unsafe) {
        if (!con) {
          error_("No context.", __FUNCTION__);
          return;
        }
      }

      con->render_textedit(get_state());
    } while (0);

    up_to_date();
  }

  void textedit::set_text(u8string_view text) {
    if (m_text != text) {
      m_text = u8string(text);
      set_expired(true);
    }
  }

  void textedit::set_length_limit(sl::whole limit) {
    m_length_limit = limit;

    if (m_text.size() > limit) {
      m_text.resize(limit);
      set_expired(true);
    }
  }

  void textedit::set_cursor(sl::index cursor) {
    if (m_cursor != cursor) {
      m_cursor = cursor;
      set_expired(true);
    }
  }

  void textedit::set_selection(sl::index selection) {
    if (m_selection != selection) {
      m_selection = selection;
      set_expired(true);
    }
  }

  auto textedit::get_text() const -> u8string_view {
    return m_text;
  }

  auto textedit::get_cursor() const -> sl::index {
    return m_cursor;
  }

  auto textedit::get_selection() const -> sl::index {
    return m_selection;
  }

  auto textedit::get_state() const -> textedit_state {
    return { panel::get_state(), has_focus(),  get_text(),
             m_length_limit,     get_cursor(), get_selection() };
  }

  auto textedit::update(ptr_widget         object,
                        textedit_state     state,
                        textedit::filter   f,
                        cref_input_handler in) -> update_result {

    const auto x = in.get_cursor_x();
    const auto y = in.get_cursor_y();

    const auto has_cursor = contains(state.bounds, x, y) &&
                            (!object || object->event_allowed(x, y));

    auto event_status = false;
    auto has_focus    = state.has_focus;

    auto text      = u8string { state.text };
    auto cursor    = state.cursor;
    auto selection = state.selection;

    if (has_focus) {
      auto s = in.get_text();

      if (!s.empty()) {
        for (auto c : s) {
          if (c == ctrl_backspace) {
            sl::index n = 0;
            char32_t  buf;

            for (sl::index i = 0; i < cursor;) {
              n = i;

              if (!utf8::decode(text, i, buf))
                break;
            }

            text.erase(text.begin() + n, text.begin() + cursor);

            cursor = n;
          } else if (c != ctrl_delete) {
            if (!f || f(c)) {
              if (state.length_limit == 0 ||
                  text.size() < state.length_limit)
                if (!utf8::encode(c, text, cursor))
                  error_("Unable to encode UTF-8 string.",
                         __FUNCTION__);
            }
          }
        }
      }
    } else if (has_cursor && in.is_key_pressed(key_lbutton)) {
      event_status = true;
      has_focus    = true;
    }

    return { event_status, has_focus, text, cursor, selection };
  }

  auto textedit::textedit_tick(cref_input_handler in) -> bool {
    if (in.is_key_pressed(key_tab)) {
      if (auto p = get_parent(); p) {
        p->next_tab();

        return true;
      }
    } else {
      auto s = update(shared_from_this(), get_state(), m_filter, in);

      set_focus(s.has_focus);
      set_text(s.text);
      set_cursor(s.cursor);
      set_selection(s.selection);

      return s.event_status;
    }

    return false;
  }
}
