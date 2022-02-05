/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "button.h"

#include <utility>

namespace laplace::ui::elem {
  using core::input_event, core::input_handler,
      core::keys::key_lbutton, core::is_key_down, core::is_key_up;

  void button::on_click(event_button_click ev) noexcept {
    m_on_click = std::move(ev);
  }

  auto button::process_event(input_event const &ev) noexcept
      -> event_status {
    return button_process_event(ev);
  }

  void button::tick(sl::time             delta_msec,
                    input_handler const &in) noexcept {
    button_tick(in);
  }

  void button::render(context const &con) noexcept {
    con.render_button(get_button_state());
    up_to_date();
  }

  void button::set_pressed(bool is_pressed) noexcept {
    if (m_is_pressed != is_pressed) {
      m_is_pressed = is_pressed;
      set_expired(true);
    }
  }

  void button::set_cursor(bool has_cursor) noexcept {
    if (m_has_cursor != has_cursor) {
      m_has_cursor = has_cursor;
      set_expired(true);
    }
  }

  auto button::is_pressed() const noexcept -> bool {
    return m_is_pressed;
  }

  auto button::has_cursor() const noexcept -> bool {
    return m_has_cursor;
  }

  auto button::get_button_state() const noexcept -> button_state {
    return { panel::get_panel_state(), m_is_pressed, m_has_cursor };
  }

  auto button::update(ptr_widget const         &object,
                      button_state              button_state,
                      event_button_click const &on_button_click,
                      input_event const        &ev) noexcept
      -> update_result {
    auto const x = ev.cursor_x;
    auto const y = ev.cursor_y;

    auto const has_cursor = contains(button_state.bounds, x, y) &&
                            (!object || object->event_allowed(x, y));

    if (ev.key != key_lbutton)
      return { button_state.is_pressed };

    if (button_state.is_pressed) {
      if (is_key_up(ev)) {
        if (has_cursor && on_button_click)
          on_button_click(object);

        return { false };
      }
    } else if (is_key_down(ev) && has_cursor) {
      return { true };
    }

    return { button_state.is_pressed };
  }

  auto button::button_process_event(input_event const &ev) noexcept
      -> event_status {
    set_pressed(
        update(shared_from_this(), get_button_state(), m_on_click, ev)
            .is_pressed);
    return event_status::proceed;
  }

  void button::button_tick(input_handler const &in) noexcept {
    auto const x = in.get_cursor_x();
    auto const y = in.get_cursor_y();

    set_cursor(event_allowed(x, y) &&
               (is_pressed() || !in.is_key_down(key_lbutton)));
  }
}
