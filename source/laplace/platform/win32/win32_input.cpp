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

  sl::whole const input::default_resolution_x = 2000;
  sl::whole const input::default_resolution_y = 1500;

  input::keymap_table const default_keymap = []() noexcept {
    auto v = keymap_table {};
    for (sl::index i = 0; i < key_count; i++) v[i] = i;

    v[key_capslock_toggle]   = 0;
    v[key_numlock_toggle]    = 0;
    v[key_scrolllock_toggle] = 0;
    v[key_lshift]            = 0;
    v[key_rshift]            = 0;
    v[key_lcontrol]          = 0;
    v[key_rcontrol]          = 0;
    v[key_lalt]              = 0;
    v[key_ralt]              = 0;

    return v;
  }();

  input::input() noexcept {
    set_keymap(default_keymap);
  }

  input::~input() noexcept {
    if (!m_use_system_cursor) {
      ShowCursor(true);
    }
  }

  void input::use_system_cursor(bool use) noexcept {
    if (m_use_system_cursor == use)
      return;

    m_use_system_cursor = use;
    ShowCursor(use);
  }

  void input::set_cursor_enabled(bool is_enabled) noexcept {
    m_is_cursor_enabled = is_enabled;
  }

  void input::set_mouse_resolution(sl::whole x,
                                   sl::whole y) noexcept {
    m_mouse.res_x = x;
    m_mouse.res_y = y;
  }

  void input::set_clamp(bool clamp_x_axis,
                        bool clamp_y_axis) noexcept {
    m_clamp_x = clamp_x_axis;
    m_clamp_y = clamp_y_axis;
  }

  auto input::get_mouse_resolution_x() const noexcept -> sl::whole {
    return m_use_system_cursor ? m_window_width : m_res_x;
  }

  auto input::get_mouse_resolution_y() const noexcept -> sl::whole {
    return m_use_system_cursor ? m_window_height : m_res_y;
  }

  auto input::get_mouse_x() const noexcept -> sl::index {
    return m_mouse.x;
  }

  auto input::get_mouse_y() const noexcept -> sl::index {
    return m_mouse.y;
  }

  auto input::get_mouse_delta_x() const noexcept -> sl::index {
    return m_use_system_cursor ? get_cursor_delta_x()
                               : m_mouse.delta_x;
  }

  auto input::get_mouse_delta_y() const noexcept -> sl::index {
    return m_use_system_cursor ? get_cursor_delta_y()
                               : m_mouse.delta_y;
  }

  auto input::get_events() const noexcept -> span<const input_event> {
    return m_events;
  }
  void input::set_window_rect(sl::index x,
                              sl::index y,
                              sl::whole width,
                              sl::whole height) noexcept {

    m_center_x = x + width / 2;
    m_center_y = y + height / 2;

    m_window_width  = width;
    m_window_height = height;
  }

  void input::attach(HWND handle) noexcept {
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

  void input::process(HRAWINPUT raw_input) noexcept {
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

  void input::refresh_mouse() noexcept {
    m_mouse_state.delta_x = 0;
    m_mouse_state.delta_y = 0;
  }

  auto input::has(uint32_t flags, uint32_t flag) noexcept -> bool {
    return (flags & flag) == flag;
  }

  void input::process_mouse(void const *raw_data) noexcept {
    auto raw = *reinterpret_cast<const RAWMOUSE *>(raw_data);

    if (m_use_system_cursor) {
      auto p = POINT {};
      GetCursorPos(&p);
      ScreenToClient(m_handle, &p);

      update_cursor(p.x, p.y);

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

        if (m_is_cursor_enabled)
          update_cursor((x * m_window_width) / m_res_x,
                        (y * m_window_height) / m_res_y);
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

  void input::process_keyboard(void const *raw_data) noexcept {
    auto raw = *reinterpret_cast<const RAWKEYBOARD *>(raw_data);

    if (has(raw.Flags, RI_KEY_MAKE)) {
      process_key(static_cast<uint8_t>(raw.VKey), true);
    }

    if (has(raw.Flags, RI_KEY_BREAK)) {
      process_key(static_cast<uint8_t>(raw.VKey), false);
    }
  }
}
