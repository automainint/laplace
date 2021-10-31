/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "input.h"

#include "../../core/string.h"

namespace laplace::linux {
  using namespace core::keys;

  using core::input_event;

  sl::index const input::default_button_mapping[] = { 0, key_lbutton,
                                                      key_mbutton,
                                                      key_rbutton };

  sl::index const input::default_wheel_scale = 120;

  input::input() noexcept {
    m_button_mapping.assign(default_button_mapping,
                            default_button_mapping + 4);

    m_keyboard.resize(256);
  }

  void input::update_key(sl::index key, bool is_down) noexcept {
    if (key < 0 || key >= m_keyboard.size()) {
      error_(fmt("Invalid key code %d.", (int) key), __FUNCTION__);
      return;
    }

    if (m_keyboard[key].is_down == is_down)
      return;

    m_events.emplace_back<input_event>(
        { .cursor_x  = m_cursor_x,
          .cursor_y  = m_cursor_y,
          .key       = key,
          .delta     = is_down ? input_event::delta_key_down
                               : input_event::delta_key_up,
          .character = U'\0' });

    m_keyboard[key].is_down = is_down;
  }

  void input::update_button(sl::index button, bool is_down) noexcept {
    if (button < 0 || button >= m_button_mapping.size() ||
        m_button_mapping[button] == 0) {
      error_(fmt("Invalid button code %d.", (int) button),
             __FUNCTION__);
      return;
    }

    update_key(m_button_mapping[button], is_down);
  }

  void input::update_wheel(sl::index direction) noexcept {
    m_events.emplace_back<input_event>(
        { .cursor_x  = m_cursor_x,
          .cursor_y  = m_cursor_y,
          .key       = key_wheel,
          .delta     = direction * m_wheel_scale,
          .character = U'\0' });

    m_wheel_delta += direction * m_wheel_scale;
  }

  void input::update_cursor(sl::index x, sl::index y) noexcept {
    m_mouse_delta_x += x - m_cursor_x;
    m_mouse_delta_y += y - m_cursor_y;

    m_cursor_x = x;
    m_cursor_y = y;
  }

  void input::use_system_cursor(bool) noexcept { }

  void input::set_cursor_enabled(bool) noexcept { }

  void input::set_mouse_resolution(sl::whole x,
                                   sl::whole y) noexcept { }

  void input::set_clamp(bool, bool) noexcept { }

  auto input::is_capslock() const noexcept -> bool {
    return false;
  }

  auto input::is_numlock() const noexcept -> bool {
    return false;
  }

  auto input::is_scrolllock() const noexcept -> bool {
    return false;
  }

  auto input::is_alt() const noexcept -> bool {
    return false;
  }

  auto input::is_shift() const noexcept -> bool {
    return false;
  }

  auto input::is_control() const noexcept -> bool {
    return false;
  }

  auto input::is_key_down(sl::index n) const noexcept -> bool {
    if (n < 0 || n >= m_keyboard.size())
      return false;

    return m_keyboard[n].is_down;
  }

  auto input::is_key_up(sl::index n) const noexcept -> bool {
    if (n < 0 || n >= m_keyboard.size())
      return false;

    return !m_keyboard[n].is_down;
  }

  auto input::get_mouse_resolution_x() const noexcept -> sl::whole {
    return 0;
  }

  auto input::get_mouse_resolution_y() const noexcept -> sl::whole {
    return 0;
  }

  auto input::get_mouse_x() const noexcept -> sl::index {
    return m_cursor_x;
  }

  auto input::get_mouse_y() const noexcept -> sl::index {
    return m_cursor_y;
  }

  auto input::get_mouse_delta_x() const noexcept -> sl::index {
    return m_mouse_delta_x;
  }

  auto input::get_mouse_delta_y() const noexcept -> sl::index {
    return m_mouse_delta_y;
  }

  auto input::get_cursor_x() const noexcept -> sl::index {
    return m_cursor_x;
  }

  auto input::get_cursor_y() const noexcept -> sl::index {
    return m_cursor_y;
  }

  auto input::get_wheel_delta() const noexcept -> sl::index {
    return m_wheel_delta;
  }

  auto input::get_events() const noexcept
      -> std::span<core::input_event const> {
    return m_events;
  }

  void input::refresh() noexcept {
    m_mouse_delta_x = 0;
    m_mouse_delta_y = 0;
    m_wheel_delta   = 0;

    m_events.clear();
  }
}
