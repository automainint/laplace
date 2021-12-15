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

#include <utility>

namespace laplace::ui::elem {
  using core::cref_input_handler, core::keys::key_lbutton,
      core::is_key_down, core::is_key_up;

  void button::on_click(event_button_click ev) noexcept {
    m_on_click = std::move(ev);
  }

  void button::tick(sl::time           delta_msec,
                    cref_input_handler in) noexcept {
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
                      cref_input_handler        in) noexcept
      -> update_result {
    auto is_pressed = button_state.is_pressed;

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
          if (has_cursor && on_button_click)
            on_button_click(object);

          is_pressed = false;
        }
      } else if (is_key_down(ev) && has_cursor) {
        is_pressed = true;
      }
    }

    auto const x = in.get_cursor_x();
    auto const y = in.get_cursor_y();

    auto const has_cursor = contains(button_state.bounds, x, y) &&
                            (!object ||
                             object->event_allowed(x, y)) &&
                            (is_pressed ||
                             !in.is_key_down(key_lbutton));

    return { is_pressed, has_cursor };
  }

  void button::button_tick(cref_input_handler in) noexcept {
    auto status = update(shared_from_this(), get_button_state(),
                         m_on_click, in);

    set_pressed(status.is_pressed);
    set_cursor(status.has_cursor);
  }
}
