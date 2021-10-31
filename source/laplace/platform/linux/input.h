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

#include "../../core/input_handler.h"
#include "xlib.h"

namespace laplace::linux {
  class input {
  public:
    input(input const &) = delete;
    input(input &&)      = delete;
    auto operator=(input const &) -> input & = delete;
    auto operator=(input &&) -> input & = delete;

    struct key_state {
      bool is_down = false;
    };

    static sl::index const default_button_mapping[];
    static sl::index const default_wheel_scale;

    input() noexcept;
    ~input() noexcept = default;

    void update_key(sl::index key, bool is_down) noexcept;
    void update_button(sl::index button, bool is_down) noexcept;
    void update_wheel(sl::index direction) noexcept;
    void update_cursor(sl::index x, sl::index y) noexcept;

    void use_system_cursor(bool) noexcept;
    void set_cursor_enabled(bool) noexcept;
    void set_mouse_resolution(sl::whole x, sl::whole y) noexcept;
    void set_clamp(bool, bool) noexcept;

    [[nodiscard]] auto is_capslock() const noexcept -> bool;
    [[nodiscard]] auto is_numlock() const noexcept -> bool;
    [[nodiscard]] auto is_scrolllock() const noexcept -> bool;
    [[nodiscard]] auto is_alt() const noexcept -> bool;
    [[nodiscard]] auto is_shift() const noexcept -> bool;
    [[nodiscard]] auto is_control() const noexcept -> bool;

    [[nodiscard]] auto is_key_down(sl::index) const noexcept -> bool;
    [[nodiscard]] auto is_key_up(sl::index) const noexcept -> bool;

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

    [[nodiscard]] auto get_cursor_x() const noexcept -> sl::index;
    [[nodiscard]] auto get_cursor_y() const noexcept -> sl::index;
    [[nodiscard]] auto get_wheel_delta() const noexcept -> sl::index;

    [[nodiscard]] auto get_events() const noexcept
        -> std::span<core::input_event const>;

    void refresh() noexcept;

  private:
    sl::vector<sl::index>         m_button_mapping;
    sl::vector<key_state>         m_keyboard;
    sl::vector<core::input_event> m_events;

    sl::index m_cursor_x      = 0;
    sl::index m_cursor_y      = 0;
    sl::index m_mouse_delta_x = 0;
    sl::index m_mouse_delta_y = 0;
    sl::index m_wheel_scale   = default_wheel_scale;
    sl::index m_wheel_delta   = 0;
  };
}

#endif
