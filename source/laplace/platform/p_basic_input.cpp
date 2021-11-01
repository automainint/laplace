/*  laplace/platform/c_basic_input.cpp
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

#include "basic_input.h"

#include "../core/string.h"

namespace laplace::platform {
  using std::span, core::input_event;
  using namespace core::keys;

  sl::time const basic_input::default_char_predelay_msec = 500;
  sl::time const basic_input::default_char_period_msec   = 50;

  auto basic_input::is_key_down(sl::index code) const noexcept
      -> bool {
    if (code < 0 || code >= key_count)
      return false;

    return m_keyboard[code].is_down;
  }

  auto basic_input::is_key_up(sl::index code) const noexcept -> bool {
    return !is_key_down(code);
  }

  auto basic_input::is_capslock() const noexcept -> bool {
    return is_key_down(key_capslock);
  }

  auto basic_input::is_numlock() const noexcept -> bool {
    return is_key_down(key_numlock);
  }

  auto basic_input::is_scrolllock() const noexcept -> bool {
    return is_key_down(key_scrolllock);
  }

  auto basic_input::is_alt() const noexcept -> bool {
    return is_key_down(key_alt);
  }

  auto basic_input::is_shift() const noexcept -> bool {
    return is_key_down(key_shift);
  }

  auto basic_input::is_control() const noexcept -> bool {
    return is_key_down(key_control);
  }

  auto basic_input::get_cursor_x() const noexcept -> sl::index {
    return m_cursor.x;
  }

  auto basic_input::get_cursor_y() const noexcept -> sl::index {
    return m_cursor.y;
  }

  auto basic_input::get_cursor_delta_x() const noexcept -> sl::index {
    return m_cursor.delta_x;
  }

  auto basic_input::get_cursor_delta_y() const noexcept -> sl::index {
    return m_cursor.delta_y;
  }

  auto basic_input::get_wheel_delta() const noexcept -> sl::index {
    return m_wheel_delta;
  }

  auto basic_input::get_events() const noexcept
      -> span<const input_event> {
    return m_events;
  }

  void basic_input::update_key(sl::index key, bool is_down) noexcept {
    auto const code = map_key(key);

    if (m_keyboard[code].is_down == is_down)
      return;

    m_events.emplace_back<input_event>(
        { .cursor_x  = get_cursor_x(),
          .cursor_y  = get_cursor_y(),
          .key       = code,
          .delta     = is_down ? input_event::delta_key_down
                               : input_event::delta_key_up,
          .character = process_char(code, is_down) });

    m_keyboard[code].is_down = is_down;
  }

  void basic_input::update_wheel(sl::index delta) noexcept {
    m_wheel_delta += delta * m_wheel_scale;

    m_events.emplace_back<input_event>(
        { .cursor_x = get_cursor_x(),
          .cursor_y = get_cursor_y(),
          .key      = key_wheel,
          .delta    = delta * m_wheel_scale });
  }

  void basic_input::update_cursor(sl::index x, sl::index y) noexcept {
    m_cursor.delta_x += x - m_cursor.x;
    m_cursor.delta_y += y - m_cursor.y;
    m_cursor.x = x;
    m_cursor.y = y;
  }

  void basic_input::set_char_predelay(
      sl::time predelay_msec) noexcept {
    m_char_predelay_msec = predelay_msec;
  }

  void basic_input::set_char_period(sl::time period_msec) noexcept {
    m_char_period_msec = period_msec;
  }

  void basic_input::set_keymap(keymap_table keymap) noexcept {
    m_keymap = keymap;
  }

  void basic_input::set_wheel_scale(sl::index scale) noexcept {
    m_wheel_scale = scale;
  }

  void basic_input::tick(sl::time delta_msec) noexcept {
    if (!m_is_char_pressed)
      return;

    m_char_time_msec -= delta_msec;

    if (m_char_time_msec <= 0) {
      m_events.emplace_back<input_event>(
          { .cursor_x  = get_cursor_x(),
            .cursor_y  = get_cursor_y(),
            .character = m_last_char });

      m_char_time_msec += m_char_period_msec;
    }
  }

  void basic_input::refresh() noexcept {
    m_cursor.delta_x = 0;
    m_cursor.delta_y = 0;
    m_wheel_delta    = 0;

    m_events.clear();
  }

  void basic_input::reset_keyboard() noexcept {
    for (auto &k : m_keyboard) k.is_down = false;
  }

  auto basic_input::map_key(sl::index key) noexcept -> uint8_t {
    if (key < 0 || key >= key_count)
      return 0;
    return m_keymap[key];
  }

  auto basic_input::to_char(uint8_t key) noexcept -> char32_t {
    if (key >= key_numpad0 && key <= key_numpad9) {
      if (is_numlock() && !is_shift())
        return U'0' + (key - key_numpad0);
      if (key == key_numpad4)
        return ctrl_left;
      if (key == key_numpad8)
        return ctrl_up;
      if (key == key_numpad6)
        return ctrl_right;
      if (key == key_numpad2)
        return ctrl_down;

    } else if (key >= key_a && key <= key_z) {
      if (is_shift() == is_capslock())
        return U'a' + (key - key_a);
      return U'A' + (key - key_a);

    } else if (key >= key_0 && key <= key_9) {
      if (!is_shift())
        return U'0' + (key - key_0);

      constexpr auto chars = U")!@#$%^&*(";
      return chars[key - key_0];

    } else if (key == key_space) {
      return U' ';
    } else if (key == key_multiply) {
      return U'*';
    } else if (key == key_add) {
      return U'+';
    } else if (key == key_subtract) {
      return U'-';
    } else if (key == key_decimal) {
      return is_numlock() && !is_shift() ? U'.' : ctrl_delete;
    } else if (key == key_divide) {
      return U'/';
    } else if (key >= key_semicolon && key <= key_tilda) {
      constexpr auto lower = U";=,-./`";
      constexpr auto upper = U":+<_>?~";

      auto const n = key - key_semicolon;
      return is_shift() ? upper[n] : lower[n];

    } else if (key >= key_open && key <= key_quote) {
      constexpr auto lower = U"[\\]'";
      constexpr auto upper = U"{|}\"";

      auto const n = key - key_open;
      return is_shift() ? upper[n] : lower[n];

    } else if (key == key_tab) {
      return ctrl_tab;
    } else if (key == key_backspace) {
      return ctrl_backspace;
    } else if (key == key_enter) {
      return ctrl_return;
    } else if (key == key_delete) {
      return ctrl_delete;
    } else if (key == key_left) {
      return ctrl_left;
    } else if (key == key_up) {
      return ctrl_up;
    } else if (key == key_right) {
      return ctrl_right;
    } else if (key == key_down) {
      return ctrl_down;
    }

    return U'\0';
  }

  auto basic_input::process_char(uint8_t key, bool is_down) noexcept
      -> char32_t {
    auto const c = to_char(key);

    if (c != U'\0') {
      m_char_time_msec = m_char_predelay_msec;

      if (is_down) {
        m_is_char_pressed = true;
        m_last_char_key   = key;
        m_last_char       = c;
        return c;
      }
      
      if (key == m_last_char_key)
        m_is_char_pressed = false;
    }

    return {};
  }
}
