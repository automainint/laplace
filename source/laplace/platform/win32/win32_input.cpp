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

#include "input.h"

#include "../../core/keys.h"
#include "../../core/utf8.h"
#include <iostream>

namespace laplace::win32 {
  using std::u8string, std::u8string_view, std::span,
      core::input_event;
  using namespace core::keys;

  const sl::time  input::char_predelay_msec   = 500;
  const sl::time  input::char_period_msec     = 50;
  const sl::whole input::default_resolution_x = 2000;
  const sl::whole input::default_resolution_y = 1500;

  input::input() {
    for (sl::index i = 0; i < key_count; i++) {
      m_keymap[i] = static_cast<uint8_t>(i);

      m_keyboard_state[i].is_down = false;
    }
  }

  input::~input() {
    if (!m_use_system_cursor) {
      ShowCursor(true);
    }
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

  void input::set_mouse_resolution(sl::whole x, sl::whole y) {
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

  auto input::is_key_down(sl::index code) const -> bool {
    if (code < 0 || code >= key_count) {
      error_("Invalid key code.", __FUNCTION__);
      return false;
    }

    return m_keyboard_state[code].is_down;
  }

  auto input::is_key_up(sl::index code) const -> bool {
    if (code < 0 || code >= key_count) {
      error_("Invalid key code.", __FUNCTION__);
      return false;
    }

    return !m_keyboard_state[code].is_down;
  }

  auto input::get_mouse_resolution_x() const -> sl::whole {
    return m_use_system_cursor ? m_window_width : m_res_x;
  }

  auto input::get_mouse_resolution_y() const -> sl::whole {
    return m_use_system_cursor ? m_window_height : m_res_y;
  }

  auto input::get_mouse_x() const -> sl::index {
    return m_mouse_state.x;
  }

  auto input::get_mouse_y() const -> sl::index {
    return m_mouse_state.y;
  }

  auto input::get_mouse_delta_x() const -> sl::index {
    return m_mouse_state.delta_x;
  }

  auto input::get_mouse_delta_y() const -> sl::index {
    return m_mouse_state.delta_y;
  }

  auto input::get_cursor_x() const -> sl::index {
    return m_mouse_state.cursor_x;
  }

  auto input::get_cursor_y() const -> sl::index {
    return m_mouse_state.cursor_y;
  }

  auto input::get_wheel_delta() const -> sl::index {
    return m_mouse_state.wheel_delta;
  }

  auto input::get_events() const -> span<const input_event> {
    return m_events;
  }

  void input::refresh() {
    m_events.clear();

    m_mouse_state.delta_x     = 0;
    m_mouse_state.delta_y     = 0;
    m_mouse_state.wheel_delta = 0;
  }

  void input::set_window_rect(sl::index x,
                              sl::index y,
                              sl::whole width,
                              sl::whole height) {

    m_center_x = x + width / 2;
    m_center_y = y + height / 2;

    m_window_width  = width;
    m_window_height = height;
  }

  void input::attach(HWND handle) {
    if (handle) {
      m_handle = handle;

      /*  Initial toggle states.
       */

      m_keyboard_state[key_capslock].is_down =
          (GetKeyState(VK_CAPITAL) & 1) == 1;
      m_keyboard_state[key_numlock].is_down =
          (GetKeyState(VK_NUMLOCK) & 1) == 1;
      m_keyboard_state[key_scrolllock].is_down =
          (GetKeyState(VK_SCROLL) & 1) == 1;

      /*  Initial cursor position.
       */

      if (m_use_system_cursor) {
        auto p = POINT {};
        GetCursorPos(&p);
        ScreenToClient(m_handle, &p);

        m_mouse_state.x = p.x;
        m_mouse_state.y = p.y;
      } else {
        ShowCursor(false);

        m_mouse_state.x = m_res_x / 2;
        m_mouse_state.y = m_res_y / 2;
      }

      /*  Register input devices.
       */

      RAWINPUTDEVICE rid[] = {
        { .usUsagePage = HID_USAGE_PAGE_GENERIC,
          .usUsage     = HID_USAGE_GENERIC_MOUSE,
          .dwFlags     = 0,
          .hwndTarget  = handle },
        { .usUsagePage = HID_USAGE_PAGE_GENERIC,
          .usUsage     = HID_USAGE_GENERIC_KEYBOARD,
          .dwFlags     = 0,
          .hwndTarget  = handle }
      };

      if (!RegisterRawInputDevices(rid, sizeof rid / sizeof *rid,
                                   sizeof(RAWINPUTDEVICE))) {
        error_("RegisterRawInputDevices failed.", __FUNCTION__);
      }
    }
  }

  void input::process(HRAWINPUT raw_input) {
    constexpr auto header_size = sizeof(RAWINPUTHEADER);
    uint32_t       size        = 0;

    GetRawInputData(raw_input, RID_INPUT, nullptr, &size,
                    header_size);

    auto bytes = vbyte(size);

    auto n = GetRawInputData(raw_input, RID_INPUT, bytes.data(),
                             &size, header_size);

    if (n == size) {
      const auto &raw = reinterpret_cast<const RAWINPUT &>(
          *bytes.data());

      if (raw.header.dwType == RIM_TYPEKEYBOARD) {
        process_keyboard(&raw.data.keyboard);
      } else if (raw.header.dwType == RIM_TYPEMOUSE) {
        process_mouse(&raw.data.mouse);
      } else {
        error_("Unknown input type.", __FUNCTION__);
      }
    }
  }

  void input::tick(sl::time delta_msec) {
    if (m_is_char_pressed) {
      m_char_period_msec -= delta_msec;

      if (m_char_period_msec <= 0) {
        m_events.emplace_back<input_event>(
            { .cursor_x  = m_mouse_state.cursor_x,
              .cursor_y  = m_mouse_state.cursor_y,
              .character = m_last_char });

        m_char_period_msec += char_period_msec;
      }
    }
  }

  void input::reset() {
    for (sl::whole i = 0; i < key_count; i++) {
      const auto is_changed = m_keyboard_state[i].is_down;

      m_keyboard_state[i].is_down = false;
    }
  }

  auto input::has(uint32_t flags, uint32_t flag) -> bool {
    return (flags & flag) == flag;
  }

  auto input::to_char(uint8_t key) -> char32_t {
    if (key >= key_numpad0 && key <= key_numpad9) {
      if (is_numlock() && !is_shift()) {
        return U'0' + (key - key_numpad0);
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
        return U'a' + (key - key_a);
      } else {
        return U'A' + (key - key_a);
      }
    } else if (key >= key_0 && key <= key_9) {
      if (!is_shift()) {
        return U'0' + (key - key_0);
      } else {
        constexpr auto chars = U")!@#$%^&*(";
        return chars[key - key_0];
      }
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
    } else if (key == key_devide) {
      return U'/';
    } else if (key >= key_semicolon && key <= key_tilda) {
      constexpr auto lower = U";=,-./`";
      constexpr auto upper = U":+<_>?~";

      auto n = key - key_semicolon;

      return is_shift() ? upper[n] : lower[n];
    } else if (key >= key_open && key <= key_quote) {
      constexpr auto lower = U"[\\]'";
      constexpr auto upper = U"{|}\"";

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

    return 0u;
  }

  void input::process_mouse(const void *raw_data) {
    auto raw = *reinterpret_cast<const RAWMOUSE *>(raw_data);

    if (m_use_system_cursor) {
      auto p = POINT {};
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

        auto x = static_cast<sl::index>(
            (static_cast<double>(raw.lLastX) / 65535.) *
            static_cast<double>(m_res_x));

        auto y = static_cast<sl::index>(
            (static_cast<double>(raw.lLastY) / 65535.) *
            static_cast<double>(m_res_y));

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
            x = m_res_x - 1;
        } else if (m_res_x > 0) {
          while (x < 0) { x += m_res_x; }
          while (x >= m_res_x) { x -= m_res_x; }
        }

        if (m_clamp_y) {
          if (y < 0)
            y = 0;
          else if (y >= m_res_y)
            y = m_res_y - 1;
        } else if (m_res_y > 0) {
          while (y < 0) { y += m_res_x; }
          while (y >= m_res_y) { y -= m_res_x; }
        }

        m_mouse_state.x = x;
        m_mouse_state.y = y;

        if (m_is_cursor_enabled) {
          m_mouse_state.cursor_x = (x * m_window_width) / m_res_x;
          m_mouse_state.cursor_y = (y * m_window_height) / m_res_y;
        }
      }

      SetCursorPos(static_cast<int>(m_center_x),
                   static_cast<int>(m_center_y));
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

  void input::process_wheel(sl::index delta) {
    m_mouse_state.wheel_delta += delta;

    m_events.emplace_back<input_event>(
        { .cursor_x = m_mouse_state.cursor_x,
          .cursor_y = m_mouse_state.cursor_y,
          .key      = key_wheel,
          .delta    = delta });
  }

  void input::process_key(uint8_t key, bool is_down) {
    auto const n          = m_keymap[key];
    auto      &k          = m_keyboard_state[n];
    bool const is_changed = is_down != k.is_down;

    k.is_down = is_down;

    auto const get_char = [&]() -> char32_t {
      if (is_changed)
        return process_char(n, is_down);
      return U'\0';
    };

    m_events.emplace_back<input_event>(
        { .cursor_x  = m_mouse_state.cursor_x,
          .cursor_y  = m_mouse_state.cursor_y,
          .key       = n,
          .delta     = is_down ? input_event::delta_key_down
                               : input_event::delta_key_up,
          .character = get_char() });
  }

  auto input::process_char(uint8_t key, bool is_down) -> char32_t {
    auto const c = to_char(key);

    if (c) {
      m_char_period_msec = char_predelay_msec;

      if (is_down) {
        m_is_char_pressed = true;
        m_last_char_key   = key;
        m_last_char       = c;
        return c;

      } else if (key == m_last_char_key) {
        m_is_char_pressed = false;
      }
    }

    return {};
  }
}
