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

#pragma once

#include "windef.h"
#include "../events.h"
#include <vector>
#include <string>
#include <array>
#include <cstdint>

#undef min
#undef max
#undef near
#undef far

namespace laplace::win32
{
    /*  Windows raw input implementation.
     */
    class input
    {
    public:
        static constexpr size_t char_predelay_msec      = 500;
        static constexpr size_t char_period_msec        = 50;
        static constexpr size_t default_resolution_x    = 2000;
        static constexpr size_t default_resolution_y    = 1500;

        struct key_state
        {
            bool is_down    : 1 = false;
            bool is_changed : 1 = false;
        };

        struct mouse_state
        {
            int x           = 0;
            int y           = 0;
            int delta_x     = 0;
            int delta_y     = 0;
            int cursor_x    = 0;
            int cursor_y    = 0;
            int wheel_delta = 0;
        };

        static constexpr size_t key_count = 256;

        using keyboard_state = std::array<key_state, key_count>;
        using keymap_table = std::array<uint8_t, key_count>;

        input();
        ~input();

        void on_key_down(event_key_down ev);
        void on_wheel(event_wheel ev);

        void use_system_cursor(bool use);
        void set_cursor_enabled(bool is_enabled);
        void set_mouse_resolution(size_t x, size_t y);
        void set_window_rect(size_t x, size_t y, size_t width, size_t height);
        void set_clamp(bool clamp_x_axis, bool clamp_y_axis);

        void attach(HWND handle);

        auto is_capslock() const -> bool;
        auto is_numlock() const -> bool;
        auto is_scrolllock() const -> bool;

        auto is_alt() const -> bool;
        auto is_shift() const -> bool;
        auto is_control() const -> bool;

        auto is_key_down(int code) const -> bool;
        auto is_key_up(int code) const -> bool;
        auto is_key_changed(int code) const -> bool;
        auto is_key_pressed(int code) const -> bool;
        auto is_key_unpressed(int code) const -> bool;

        auto get_mouse_resolution_x() const -> size_t;
        auto get_mouse_resolution_y() const -> size_t;

        auto get_mouse_x() const -> int;
        auto get_mouse_y() const -> int;
        auto get_mouse_delta_x() const -> int;
        auto get_mouse_delta_y() const -> int;

        auto get_cursor_x() const -> int;
        auto get_cursor_y() const -> int;

        auto get_wheel_delta() const -> int;

        auto get_text() const -> std::u8string_view;

        void process(HRAWINPUT raw_input);
        void tick(size_t delta_msec);
        void refresh();

    private:
        static auto has(uint32_t flags, uint32_t flag) -> bool;
        auto to_char(uint8_t key) -> char32_t;

        void process_mouse(const void *raw_data);
        void process_keyboard(const void *raw_data);

        void process_wheel(int delta);
        void process_key(uint8_t key, bool is_down);
        void process_char(uint8_t key, bool is_down);

        event_key_down  m_on_key_down;
        event_wheel     m_on_wheel;
        keymap_table    m_keymap;
        keyboard_state  m_keyboard_state;
        mouse_state     m_mouse_state;
        size_t          m_res_x             = default_resolution_x;
        size_t          m_res_y             = default_resolution_y;
        int             m_center_x          = 0;
        int             m_center_y          = 0;
        size_t          m_window_width      = 0;
        size_t          m_window_height     = 0;
        bool            m_clamp_x           = true;
        bool            m_clamp_y           = true;
        bool            m_use_system_cursor = true;
        bool            m_is_cursor_enabled = true;
        bool            m_is_char_pressed   = false;
        char32_t        m_last_char         = 0;
        uint8_t         m_last_char_key     = 0;
        size_t          m_char_period_msec  = char_predelay_msec;
        std::u8string   m_text;
        HWND            m_handle            = nullptr;
    };
}
