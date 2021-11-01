/*  laplace/platform/linux/input.h
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

#ifndef laplace_platform_linux_input_h
#define laplace_platform_linux_input_h

#include "../basic_input.h"
#include "xlib.h"

namespace laplace::linux {
  class input : public platform::basic_input {
  public:
    static sl::index const default_wheel_scale;

    input() noexcept;
    ~input() noexcept override = default;

    void use_system_cursor(bool) noexcept;
    void set_cursor_enabled(bool) noexcept;
    void set_mouse_resolution(sl::whole, sl::whole) noexcept;
    void set_clamp(bool, bool) noexcept;

    [[nodiscard]] auto get_mouse_resolution_x() const noexcept
        -> sl::whole;
    [[nodiscard]] auto get_mouse_resolution_y() const noexcept
        -> sl::whole;

    [[nodiscard]] auto get_mouse_x() const noexcept -> sl::index;
    [[nodiscard]] auto get_mouse_y() const noexcept -> sl::index;

    [[nodiscard]] auto get_mouse_delta_x() const noexcept
        -> sl::index;
    [[nodiscard]] auto get_mouse_delta_y() const noexcept
        -> sl::index;

    void update_button(sl::index button, bool is_down) noexcept;
    void update_controls(sl::index key, bool is_down) noexcept;

    void init_keymap(Display *display) noexcept;

  private:
    [[nodiscard]] static auto load_keymap(Display *display) noexcept
        -> keymap_table;

    void adjust_and_set_keymap(keymap_table v) noexcept;

    void toggle(sl::index key, bool is_down, bool &state) noexcept;

    bool m_toggle_capslock   = false;
    bool m_toggle_numlock    = false;
    bool m_toggle_scrolllock = false;

    uint8_t in_lbutton    = 0;
    uint8_t in_rbutton    = 0;
    uint8_t in_mbutton    = 0;
    uint8_t in_xbutton1   = 0;
    uint8_t in_xbutton2   = 0;
    uint8_t in_shift      = 0;
    uint8_t in_control    = 0;
    uint8_t in_alt        = 0;
    uint8_t in_capslock   = 0;
    uint8_t in_numlock    = 0;
    uint8_t in_scrolllock = 0;
  };
}

#endif
