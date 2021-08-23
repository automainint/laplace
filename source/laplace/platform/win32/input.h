/*  laplace/platform/win32/input.h
 *
 *      Win32 raw input declarations.
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

#ifndef laplace_platform_win32_input_h
#define laplace_platform_win32_input_h

#include "../events.h"
#include "win.predef.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#undef min
#undef max
#undef near
#undef far

namespace laplace::win32 {
  /*  Windows raw input implementation.
   */
  class input {
  public:
    static const sl::whole char_predelay_msec;
    static const sl::whole char_period_msec;
    static const sl::whole default_resolution_x;
    static const sl::whole default_resolution_y;

    struct key_state {
      bool is_down    : 1 = false;
      bool is_changed : 1 = false;
    };

    struct mouse_state {
      sl::index x           = 0;
      sl::index y           = 0;
      sl::index delta_x     = 0;
      sl::index delta_y     = 0;
      sl::index cursor_x    = 0;
      sl::index cursor_y    = 0;
      sl::index wheel_delta = 0;
    };

    static constexpr sl::whole key_count = 256;

    using keyboard_state = std::array<key_state, key_count>;
    using keymap_table   = std::array<uint8_t, key_count>;

    input();
    ~input();

    void on_key_down(event_key_down ev);
    void on_wheel(event_wheel ev);

    void use_system_cursor(bool use);
    void set_cursor_enabled(bool is_enabled);
    void set_mouse_resolution(sl::whole x, sl::whole y);
    void set_clamp(bool clamp_x_axis, bool clamp_y_axis);

    auto is_capslock() const -> bool;
    auto is_numlock() const -> bool;
    auto is_scrolllock() const -> bool;

    auto is_alt() const -> bool;
    auto is_shift() const -> bool;
    auto is_control() const -> bool;

    auto is_key_down(sl::index code) const -> bool;
    auto is_key_up(sl::index code) const -> bool;
    auto is_key_changed(sl::index code) const -> bool;
    auto is_key_pressed(sl::index code) const -> bool;
    auto is_key_unpressed(sl::index code) const -> bool;

    auto get_mouse_resolution_x() const -> sl::whole;
    auto get_mouse_resolution_y() const -> sl::whole;

    auto get_mouse_x() const -> sl::index;
    auto get_mouse_y() const -> sl::index;
    auto get_mouse_delta_x() const -> sl::index;
    auto get_mouse_delta_y() const -> sl::index;

    auto get_cursor_x() const -> sl::index;
    auto get_cursor_y() const -> sl::index;

    auto get_wheel_delta() const -> sl::index;
    auto get_text() const -> std::u8string_view;
    void refresh();

    void set_window_rect(sl::index x, sl::index y, sl::whole width,
                         sl::whole height);
    void attach(HWND handle);
    void process(HRAWINPUT raw_input);
    void tick(uint64_t delta_msec);
    void reset();

  private:
    static auto has(uint32_t flags, uint32_t flag) -> bool;
    auto        to_char(uint8_t key) -> char32_t;

    void process_mouse(const void *raw_data);
    void process_keyboard(const void *raw_data);

    void process_wheel(int delta);
    void process_key(uint8_t key, bool is_down);
    void process_char(uint8_t key, bool is_down);

    event_key_down m_on_key_down;
    event_wheel    m_on_wheel;
    keymap_table   m_keymap;
    keyboard_state m_keyboard_state;
    mouse_state    m_mouse_state;
    sl::whole      m_res_x             = default_resolution_x;
    sl::whole      m_res_y             = default_resolution_y;
    sl::index      m_center_x          = 0;
    sl::index      m_center_y          = 0;
    sl::whole      m_window_width      = 0;
    sl::whole      m_window_height     = 0;
    bool           m_clamp_x           = true;
    bool           m_clamp_y           = true;
    bool           m_use_system_cursor = true;
    bool           m_is_cursor_enabled = true;
    bool           m_is_char_pressed   = false;
    char32_t       m_last_char         = 0;
    uint8_t        m_last_char_key     = 0;
    sl::whole      m_char_period_msec  = char_predelay_msec;
    std::u8string  m_text;
    HWND           m_handle = nullptr;
  };
}

#endif
