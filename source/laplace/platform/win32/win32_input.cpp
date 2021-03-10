/*  laplace/platform/win32/win32_input.cpp
 *
 *      Win32 raw input implementation.
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

#define WIN32_LEAN_AND_MEAN
#define laplace_windows_header
#include <windows.h>

#include <hidusage.h>

#undef min
#undef max

#include "../../core/utils.h"
#include "../keys.h"
#include "input.h"
#include <cassert>
#include <iostream>

namespace laplace::win32 {
  using std::u8string_view;
  using namespace platform;

  input::input() {
    for (size_t i = 0; i < key_count; i++) {
      m_keymap[i] = static_cast<uint8_t>(i);

      m_keyboard_state[i].is_down    = false;
      m_keyboard_state[i].is_changed = false;
    }
  }

  input::~input() {
    if (!m_use_system_cursor) {
      ShowCursor(true);
    }
  }

  void input::on_key_down(event_key_down ev) {
    m_on_key_down = ev;
  }

  void input::on_wheel(event_wheel ev) {
    m_on_wheel = ev;
  }

  void input::use_system_cursor(bool use) {
    if (m_use_system_cursor != use) {
      m_use_system_cursor = use;

      ShowCursor(use);
    }
  }

  void input::set_cursor_enabled(bool is_enabled) {
    m_is_cursor_enabled = is_enabled;
  }

  void input::set_mouse_resolution(size_t x, size_t y) {
    m_res_x = x;
    m_res_y = y;
  }

  void input::set_clamp(bool clamp_x_axis, bool clamp_y_axis) {
    m_clamp_x = clamp_x_axis;
    m_clamp_y = clamp_y_axis;
  }

  auto input::is_capslock() const -> bool {
    return m_keyboard_state[key_capslock].is_down;
  }

  auto input::is_numlock() const -> bool {
    return m_keyboard_state[key_numlock].is_down;
  }

  auto input::is_scrolllock() const -> bool {
    return m_keyboard_state[key_scrolllock].is_down;
  }

  auto input::is_alt() const -> bool {
    return m_keyboard_state[key_alt].is_down;
  }

  auto input::is_shift() const -> bool {
    return m_keyboard_state[key_shift].is_down;
  }

  auto input::is_control() const -> bool {
    return m_keyboard_state[key_control].is_down;
  }

  auto input::is_key_down(int code) const -> bool {
    assert(code >= 0 && code < key_count);
    return m_keyboard_state[code].is_down;
  }

  auto input::is_key_up(int code) const -> bool {
    assert(code >= 0 && code < key_count);
    return !m_keyboard_state[code].is_down;
  }

  auto input::is_key_changed(int code) const -> bool {
    assert(code >= 0 && code < key_count);
    return m_keyboard_state[code].is_changed;
  }

  auto input::is_key_pressed(int code) const -> bool {
    assert(code >= 0 && code < key_count);
    return m_keyboard_state[code].is_down &&
           m_keyboard_state[code].is_changed;
  }

  auto input::is_key_unpressed(int code) const -> bool {
    assert(code >= 0 && code < key_count);
    return !m_keyboard_state[code].is_down &&
           m_keyboard_state[code].is_changed;
  }

  auto input::get_mouse_resolution_x() const -> size_t {
    return m_use_system_cursor ? m_window_width : m_res_x;
  }

  auto input::get_mouse_resolution_y() const -> size_t {
    return m_use_system_cursor ? m_window_height : m_res_y;
  }

  auto input::get_mouse_x() const -> int {
    return m_mouse_state.x;
  }

  auto input::get_mouse_y() const -> int {
    return m_mouse_state.y;
  }

  auto input::get_mouse_delta_x() const -> int {
    return m_mouse_state.delta_x;
  }

  auto input::get_mouse_delta_y() const -> int {
    return m_mouse_state.delta_y;
  }

  auto input::get_cursor_x() const -> int {
    return m_mouse_state.cursor_x;
  }

  auto input::get_cursor_y() const -> int {
    return m_mouse_state.cursor_y;
  }

  auto input::get_wheel_delta() const -> int {
    return m_mouse_state.wheel_delta;
  }

  auto input::get_text() const -> u8string_view {
    return m_text;
  }

  void input::refresh() {
    m_text.clear();

    m_mouse_state.delta_x     = 0;
    m_mouse_state.delta_y     = 0;
    m_mouse_state.wheel_delta = 0;

    for (auto &k : m_keyboard_state) { k.is_changed = false; }
  }

  void input::set_window_rect(size_t x, size_t y, size_t width,
                              size_t height) {

    m_center_x = static_cast<int>(x + width / 2);
    m_center_y = static_cast<int>(y + height / 2);

    m_window_width  = width;
    m_window_height = height;
  }

  void input::attach(HWND handle) {
    if (handle) {
      m_handle = handle;

      /*  Initial toggle states.
       */

      m_keyboard_state[key_capslock].is_down = (GetKeyState(VK_CAPITAL) &
                                                1) == 1;
      m_keyboard_state[key_numlock].is_down = (GetKeyState(VK_NUMLOCK) &
                                               1) == 1;
      m_keyboard_state[key_scrolllock].is_down = (GetKeyState(VK_SCROLL) &
                                                  1) == 1;

      /*  Initial cursor position.
       */

      if (m_use_system_cursor) {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(m_handle, &p);

        m_mouse_state.x = p.x;
        m_mouse_state.y = p.y;
      } else {
        ShowCursor(false);

        m_mouse_state.x = static_cast<int>(m_res_x / 2);
        m_mouse_state.y = static_cast<int>(m_res_y / 2);
      }

      /*  Register input devices.
       */

      RAWINPUTDEVICE rid[] = { { .usUsagePage = HID_USAGE_PAGE_GENERIC,
                                 .usUsage = HID_USAGE_GENERIC_MOUSE,
                                 .dwFlags = 0,
                                 .hwndTarget = handle },
                               { .usUsagePage = HID_USAGE_PAGE_GENERIC,
                                 .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                                 .dwFlags    = 0,
                                 .hwndTarget = handle } };

      if (!RegisterRawInputDevices(rid, sizeof rid / sizeof *rid,
                                   sizeof(RAWINPUTDEVICE))) {
        error(__FUNCTION__, "RegisterRawInputDevices failed.");
      }
    }
  }

  void input::process(HRAWINPUT raw_input) {
    constexpr auto header_size = sizeof(RAWINPUTHEADER);
    uint32_t       size        = 0;

    GetRawInputData(raw_input, RID_INPUT, nullptr, &size, header_size);

    vbyte bytes(size);

    auto n = GetRawInputData(
        raw_input, RID_INPUT, bytes.data(), &size, header_size);

    if (n == size) {
      const auto &raw = reinterpret_cast<const RAWINPUT &>(
          *bytes.data());

      if (raw.header.dwType == RIM_TYPEKEYBOARD) {
        process_keyboard(&raw.data.keyboard);
      } else if (raw.header.dwType == RIM_TYPEMOUSE) {
        process_mouse(&raw.data.mouse);
      } else {
        error(__FUNCTION__, "Unknown input type.");
      }
    }
  }

  void input::tick(uint64_t delta_msec) {
    if (m_is_char_pressed) {
      if (m_char_period_msec <= delta_msec) {
        size_t offset = m_text.length();
        if (!utf8_encode(m_last_char, m_text, offset))
          error(__FUNCTION__, "Unable to encode UTF-8 string.");
        m_char_period_msec += char_period_msec > delta_msec
                                  ? char_period_msec - delta_msec
                                  : char_period_msec;
      } else {
        m_char_period_msec -= delta_msec;
      }
    }
  }

  void input::reset() {
    for (size_t i = 0; i < key_count; i++) {
      const auto is_changed = m_keyboard_state[i].is_down;

      m_keyboard_state[i].is_down    = false;
      m_keyboard_state[i].is_changed = is_changed;
    }
  }

  auto input::has(uint32_t flags, uint32_t flag) -> bool {
    return (flags & flag) == flag;
  }

  auto input::to_char(uint8_t key) -> char32_t {
    if (key >= key_numpad0 && key <= key_numpad9) {
      if (is_numlock() && !is_shift()) {
        return '0' + (key - key_numpad0);
      } else if (key == key_numpad4) {
        return ctrl_left;
      } else if (key == key_numpad8) {
        return ctrl_up;
      } else if (key == key_numpad6) {
        return ctrl_right;
      } else if (key == key_numpad2) {
        return ctrl_down;
      }
    } else if (key >= key_a && key <= key_z) {
      if (is_shift() == is_capslock()) {
        return 'a' + (key - key_a);
      } else {
        return 'A' + (key - key_a);
      }
    } else if (key >= key_0 && key <= key_9) {
      if (!is_shift()) {
        return '0' + (key - key_0);
      } else {
        constexpr auto chars = ")!@#$%^&*(";
        return chars[key - key_0];
      }
    } else if (key == key_space) {
      return ' ';
    } else if (key == key_multiply) {
      return '*';
    } else if (key == key_add) {
      return '+';
    } else if (key == key_subtract) {
      return '-';
    } else if (key == key_decimal) {
      return is_numlock() && !is_shift() ? '.' : ctrl_delete;
    } else if (key == key_devide) {
      return '/';
    } else if (key >= key_semicolon && key <= key_tilda) {
      constexpr auto lower = ";=,-./`";
      constexpr auto upper = ":+<_>?~";

      auto n = key - key_semicolon;

      return is_shift() ? upper[n] : lower[n];
    } else if (key >= key_open && key <= key_quote) {
      constexpr auto lower = "[\\]'";
      constexpr auto upper = "{|}\"";

      auto n = key - key_open;

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

    return 0;
  }

  void input::process_mouse(const void *raw_data) {
    auto raw = *reinterpret_cast<const RAWMOUSE *>(raw_data);

    if (m_use_system_cursor) {
      POINT p;
      GetCursorPos(&p);
      ScreenToClient(m_handle, &p);

      m_mouse_state.delta_x += p.x - m_mouse_state.x;
      m_mouse_state.delta_y += p.y - m_mouse_state.y;

      m_mouse_state.x = m_mouse_state.cursor_x = p.x;
      m_mouse_state.y = m_mouse_state.cursor_y = p.y;
    } else {
      if (has(raw.usFlags, MOUSE_MOVE_ABSOLUTE)) {
        /*  Absolute cursor position.
         */

        auto x = static_cast<int>((raw.lLastX / 65535.) * m_res_x);
        auto y = static_cast<int>((raw.lLastY / 65535.) * m_res_y);

        m_mouse_state.delta_x += x - m_mouse_state.x;
        m_mouse_state.delta_y += y - m_mouse_state.y;

        m_mouse_state.x = x;
        m_mouse_state.y = y;
      } else {
        /*  Relative cursor position.
         */

        m_mouse_state.delta_x += raw.lLastX;
        m_mouse_state.delta_y += raw.lLastY;

        auto x = m_mouse_state.x + raw.lLastX;
        auto y = m_mouse_state.y + raw.lLastY;

        if (m_clamp_x) {
          if (x < 0)
            x = 0;
          else if (x >= m_res_x)
            x = static_cast<int>(m_res_x - 1);
        } else if (m_res_x > 0) {
          while (x < 0) { x += static_cast<int>(m_res_x); }
          while (x >= m_res_x) { x -= static_cast<int>(m_res_x); }
        }

        if (m_clamp_y) {
          if (y < 0)
            y = 0;
          else if (y >= m_res_y)
            y = static_cast<int>(m_res_y - 1);
        } else if (m_res_y > 0) {
          while (y < 0) { y += static_cast<int>(m_res_x); }
          while (y >= m_res_y) { y -= static_cast<int>(m_res_x); }
        }

        m_mouse_state.x = x;
        m_mouse_state.y = y;

        if (m_is_cursor_enabled) {
          m_mouse_state.cursor_x = static_cast<int>(
              (x * m_window_width) / m_res_x);
          m_mouse_state.cursor_y = static_cast<int>(
              (y * m_window_height) / m_res_y);
        }
      }

      SetCursorPos(m_center_x, m_center_y);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_WHEEL)) {
      process_wheel(static_cast<int16_t>(raw.usButtonData));
    }

    if (has(raw.usButtonFlags, RI_MOUSE_LEFT_BUTTON_DOWN)) {
      process_key(VK_LBUTTON, true);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_LEFT_BUTTON_UP)) {
      process_key(VK_LBUTTON, false);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_RIGHT_BUTTON_DOWN)) {
      process_key(VK_RBUTTON, true);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_RIGHT_BUTTON_UP)) {
      process_key(VK_RBUTTON, false);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_MIDDLE_BUTTON_DOWN)) {
      process_key(VK_MBUTTON, true);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_MIDDLE_BUTTON_UP)) {
      process_key(VK_MBUTTON, false);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_BUTTON_4_DOWN)) {
      process_key(VK_XBUTTON1, true);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_BUTTON_4_UP)) {
      process_key(VK_XBUTTON1, false);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_BUTTON_5_DOWN)) {
      process_key(VK_XBUTTON2, true);
    }

    if (has(raw.usButtonFlags, RI_MOUSE_BUTTON_5_UP)) {
      process_key(VK_XBUTTON2, false);
    }
  }

  void input::process_keyboard(const void *raw_data) {
    auto raw = *reinterpret_cast<const RAWKEYBOARD *>(raw_data);

    if (has(raw.Flags, RI_KEY_MAKE)) {
      process_key(static_cast<uint8_t>(raw.VKey), true);
    }

    if (has(raw.Flags, RI_KEY_BREAK)) {
      process_key(static_cast<uint8_t>(raw.VKey), false);
    }
  }

  void input::process_wheel(int delta) {
    m_mouse_state.wheel_delta += delta;

    if (m_on_wheel) {
      m_on_wheel(delta);
    }
  }

  void input::process_key(uint8_t key, bool is_down) {
    auto  n = m_keymap[key];
    auto &k = m_keyboard_state[n];

    k.is_changed = k.is_down != is_down;
    k.is_down    = is_down;

    if (k.is_changed) {
      if (is_down) {
        if (m_on_key_down) {
          m_on_key_down(n);
        }
      }

      process_char(n, is_down);
    }
  }

  void input::process_char(uint8_t key, bool is_down) {
    auto c = to_char(key);

    if (c) {
      m_char_period_msec = char_predelay_msec;

      if (is_down) {
        size_t offset = m_text.length();
        if (!utf8_encode(c, m_text, offset))
          error(__FUNCTION__, "Unable to encode UTF-8 string.");

        m_is_char_pressed = true;
        m_last_char_key   = key;
        m_last_char       = c;
      } else if (key == m_last_char_key) {
        m_is_char_pressed = false;
      }
    }
  }
}
