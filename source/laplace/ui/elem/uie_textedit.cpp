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

  using std::u8string_view, std::u8string, core::is_key_down,
      core::input_event, core::input_handler;

  sl::whole textedit::default_length_limit = 24;
  sl::whole textedit::default_flash_on     = 300;
  sl::whole textedit::default_flash_off    = 300;

  textedit::textedit() noexcept {
    set_handler(true);
  }

  void textedit::setup_filter(filter f) noexcept {
    m_filter = std::move(f);
  }

  auto textedit::process_event(input_event const &ev) noexcept
      -> event_status {
    return textedit_process_event(ev);
  }

  void textedit::tick(sl::time delta_msec,
                      input_handler const &) noexcept {
    textedit_flash(delta_msec);
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

  auto textedit::get_length_limit() const noexcept -> sl::whole {
    return m_length_limit;
  }

  auto textedit::get_cursor() const noexcept -> sl::index {
    return m_cursor;
  }

  auto textedit::get_selection() const noexcept -> sl::index {
    return m_selection;
  }

  auto textedit::get_flash() const noexcept -> sl::index {
    return m_flash;
  }

  auto textedit::get_textedit_state() const noexcept
      -> textedit_state {
    return { panel::get_panel_state(),
             has_focus(),
             get_text(),
             get_length_limit(),
             get_cursor(),
             get_selection(),
             get_flash() };
  }

  auto textedit::update(ptr_widget const &object,
                        textedit_state state, filter const &f,
                        input_event const &ev,
                        log_handler log) noexcept -> update_result {

    auto has_focus = state.has_focus;

    auto text      = u8string { state.text };
    auto cursor    = state.cursor;
    auto selection = state.selection;

    auto const x = ev.cursor_x;
    auto const y = ev.cursor_y;

    auto const has_cursor = contains(state.bounds, x, y) &&
                            (!object || object->event_allowed(x, y));

    if (has_focus) {
      if (ev.character == ctrl_backspace) {
        if (cursor > 0) {
          auto const n = cursor;
          cursor       = utf8::previous(text, cursor);
          text.erase(text.begin() + cursor, text.begin() + n);
        }
      } else if (ev.character == ctrl_delete) {
        if (cursor < text.size())
          text.erase(text.begin() + cursor,
                     text.begin() + utf8::next(text, cursor));

      } else if (ev.character == ctrl_return) {
      } else if (ev.character == ctrl_left) {
        if (cursor > 0)
          cursor = utf8::previous(text, cursor);
      } else if (ev.character == ctrl_right) {
        if (cursor < text.size())
          cursor = utf8::next(text, cursor);
      } else if (ev.character == ctrl_up) {
        cursor = 0;
      } else if (ev.character == ctrl_down) {
        cursor = text.size();
      } else if (ev.character != U'\0') {
        if (!f || f(ev.character)) {
          if (state.length_limit == 0 ||
              text.size() < state.length_limit)
            if (!utf8::encode(ev.character, text, cursor))
              log(log_event::error, "Unable to encode UTF-8 string.",
                  __FUNCTION__);
        }
      }

    } else if (has_cursor && ev.key == key_lbutton &&
               is_key_down(ev)) {
      has_focus = true;
    }

    return { has_focus, text, cursor, selection };
  }

  void textedit::textedit_set_focus(bool has_focus_) noexcept {
    if (has_focus() != has_focus_)
      set_flash(-m_flash_on - m_flash_off);

    set_focus(has_focus_);
  }

  void textedit::set_flash(sl::index flash) noexcept {
    if (has_focus() && (m_flash < 0) != (flash < 0))
      set_expired(true);

    m_flash = flash;
  }

  void textedit::textedit_flash(sl::time delta_msec) noexcept {
    auto flash = get_flash() + delta_msec;
    if (flash >= m_flash_off)
      flash -= m_flash_on + m_flash_off;
    set_flash(flash);
  }

  auto textedit::textedit_process_event(
      input_event const &ev) noexcept -> event_status {

    if (ev.character == key_tab && has_focus()) {
      if (auto p = get_parent(); p)
        p->next_tab();
      return event_status::remove;
    }

    auto const s = update(shared_from_this(), get_textedit_state(),
                          m_filter, ev, log);

    textedit_set_focus(s.has_focus);
    set_text(s.text);
    set_cursor(s.cursor);
    set_selection(s.selection);

    return event_status::proceed;
  }
}
