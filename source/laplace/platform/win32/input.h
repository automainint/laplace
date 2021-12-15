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

#include "../basic_input.h"
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
  class input : public platform::basic_input {
  public:
    static sl::whole const default_resolution_x;
    static sl::whole const default_resolution_y;

    static keymap_table const default_keymap;

    struct mouse_state {
      sl::index x       = 0;
      sl::index y       = 0;
      sl::index delta_x = 0;
      sl::index delta_y = 0;
      sl::whole res_x   = default_resolution_x;
      sl::whole res_y   = default_resolution_y;
    };

    input() noexcept;
    ~input() noexcept override;

    void use_system_cursor(bool use) noexcept;
    void set_cursor_enabled(bool is_enabled) noexcept;
    void set_mouse_resolution(sl::whole x, sl::whole y) noexcept;
    void set_clamp(bool clamp_x_axis, bool clamp_y_axis) noexcept;

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

    void set_window_rect(sl::index x,
                         sl::index y,
                         sl::whole width,
                         sl::whole height) noexcept;

    void attach(HWND handle) noexcept;
    void process(HRAWINPUT raw_input) noexcept;
    void refresh_mouse() noexcept;

  private:
    [[nodiscard]] static auto has(uint32_t flags,
                                  uint32_t flag) noexcept -> bool;

    void process_mouse(void const *raw_data) noexcept;
    void process_keyboard(void const *raw_data) noexcept;

    mouse_state m_mouse;
    sl::index   m_center_x          = 0;
    sl::index   m_center_y          = 0;
    sl::whole   m_window_width      = 0;
    sl::whole   m_window_height     = 0;
    bool        m_clamp_x           = true;
    bool        m_clamp_y           = true;
    bool        m_use_system_cursor = true;
    bool        m_is_cursor_enabled = true;

    HWND m_handle = nullptr;
  };
}

#endif
