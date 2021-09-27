/*  laplace/ui/elem/uie_button.cpp
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

#include "button.h"

#include "../../core/keys.h"
#include "../context.h"

namespace laplace::ui::elem {
  using core::cref_input_handler, core::keys::key_lbutton,
      core::is_key_down, core::is_key_up;

  void button::on_click(event_button_click ev) {
    m_on_click = ev;
  }

  auto button::tick(sl::time           delta_msec,
                    cref_input_handler in,
                    bool               is_handled) -> bool {
    return is_handled || button_tick(in);
  }

  void button::render(context const &con) {
    con.render_button(get_state());
    up_to_date();
  }

  void button::set_pressed(bool is_pressed) {
    if (m_is_pressed != is_pressed) {
      m_is_pressed = is_pressed;
      set_expired(true);
    }
  }

  void button::set_cursor(bool has_cursor) {
    if (m_has_cursor != has_cursor) {
      m_has_cursor = has_cursor;
      set_expired(true);
    }
  }

  auto button::is_pressed() const -> bool {
    return m_is_pressed;
  }

  auto button::has_cursor() const -> bool {
    return m_has_cursor;
  }

  auto button::get_state() const -> button_state {
    return { panel::get_state(), m_is_pressed, m_has_cursor };
  }

  auto button::update(ptr_widget         object,
                      button_state       button_state,
                      event_button_click on_button_click,
                      cref_input_handler in) -> update_result {
    auto event_status = false;
    auto is_pressed   = button_state.is_pressed;

    for (auto const &ev : in.get_events()) {
      if (ev.key != key_lbutton)
        continue;

      auto const x = ev.cursor_x;
      auto const y = ev.cursor_y;

      auto const has_cursor = contains(button_state.bounds, x, y) &&
                              (!object ||
                               object->event_allowed(x, y));

      if (is_pressed) {
        if (is_key_up(ev)) {
          if (has_cursor && on_button_click) {
            on_button_click(object);
          }

          event_status = true;
          is_pressed   = false;
        }
      } else if (is_key_down(ev) && has_cursor) {
        event_status = true;
        is_pressed   = true;
      }
    }

    auto const x = in.get_cursor_x();
    auto const y = in.get_cursor_y();

    auto const has_cursor = contains(button_state.bounds, x, y) &&
                            (!object ||
                             object->event_allowed(x, y)) &&
                            (is_pressed ||
                             !in.is_key_down(key_lbutton));

    return { event_status, is_pressed, has_cursor };
  }

  auto button::button_tick(cref_input_handler in) -> bool {
    auto status = update(shared_from_this(), get_state(), m_on_click,
                         in);

    set_pressed(status.is_pressed);
    set_cursor(status.has_cursor);

    return status.event_status;
  }
}
