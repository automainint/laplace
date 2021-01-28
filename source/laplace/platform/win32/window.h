/*  laplace/platform/win32/window.h
 *
 *      Win32 window declarations.
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

#include "../events.h"
#include "input.h"
#include "win.predef.h"
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#undef min
#undef max
#undef near
#undef far

namespace laplace::win32
{
    class window
    {
    public:
        using native_handle = HWND;

        static constexpr auto   class_name              = L"Laplace";
        static constexpr auto   default_window_name     = L"Laplace Application";
        static constexpr bool   default_is_visible      = false;
        static constexpr bool   default_is_fullscreen   = false;
        static constexpr size_t default_frame_width     = 960;
        static constexpr size_t default_frame_height    = 720;
        static constexpr size_t default_frame_rate      = 60;

        window();
        window(native_handle parent);

        ~window();

        void on_init(event_init ev);
        void on_cleanup(event_cleanup ev);
        void on_frame(event_frame ev);
        void on_size(event_size ev);
        void on_focus(event_focus ev);
        void on_drop_file(event_drop_file ev);

        void set_name(std::string_view name);
        void set_name(std::wstring_view name);

        void set_visible(bool is_visible);
        void set_fullscreen(bool is_fullscreen);

        void set_centered();
        void set_position(size_t x, size_t y);
        void set_size(size_t frame_width, size_t frame_height);

        void set_fullscreen_windowed();
        void set_fullscreen_mode(size_t frame_width, size_t frame_height, size_t frame_frequency);

        void set_input(std::shared_ptr<input> in);

        void set_parent(native_handle parent);
        void reset_parent();

        auto message_loop() -> int;
        void quit(int code = 0);

        auto get_screen_width() const -> size_t;
        auto get_screen_height() const -> size_t;

        auto get_fullscreen_width() const -> size_t;
        auto get_fullscreen_height() const -> size_t;

        auto get_x() const -> size_t;
        auto get_y() const -> size_t;

        auto get_width() const -> size_t;
        auto get_height() const -> size_t;

        auto get_frame_width() const -> size_t;
        auto get_frame_height() const -> size_t;

        auto is_visible() const -> bool;
        auto is_fullscreen() const -> bool;

        auto is_fullscreen_windowed() const -> bool;

        auto has_focus() const -> bool;
        auto has_cursor() const -> bool;

        auto get_native_handle() const -> native_handle;

    private:
        void init(native_handle parent);
        auto get_style_ex() const -> size_t;
        auto get_style() const -> size_t;
        void get_exe_file_name();
        void load_icon(HICON &icon, HICON &icon_sm);
        auto register_class() -> bool;
        void create_window();
        void adjust_window();
        void update_rect();
        void update_window();
        void update_display();
        void destroy_window();
        void accept_files(void *drop_data);
        auto process(UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT;

        static auto CALLBACK window_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT;

        static constexpr uint32_t default_style_ex =
            WS_EX_ACCEPTFILES;

        static constexpr uint32_t default_style =
            WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

        static constexpr uint32_t mask_fullscreen_style =
            ~(WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

        std::shared_ptr<input> m_input;

        std::wstring m_exe_file_name;
        std::wstring m_class_name;
        std::wstring m_window_name;

        size_t m_style_ex;
        size_t m_style;

        event_init      m_on_init;
        event_cleanup   m_on_cleanup;
        event_frame     m_on_frame;
        event_size      m_on_size;
        event_focus     m_on_focus;
        event_drop_file m_on_drop_file;

        size_t m_screen_x;
        size_t m_screen_y;
        size_t m_screen_width;
        size_t m_screen_height;
        size_t m_fullscreen_width;
        size_t m_fullscreen_height;
        size_t m_x;
        size_t m_y;
        size_t m_width;
        size_t m_height;
        size_t m_frame_width;
        size_t m_frame_height;
        size_t m_frame_rate;

        bool m_is_inited;
        bool m_is_visible;
        bool m_is_fullscreen;
        bool m_has_focus;
        bool m_has_cursor;

        std::mutex              m_handle_mutex;
        std::condition_variable m_window_created;
        std::thread             m_icon_loading;

        HINSTANCE m_module_handle;
        native_handle m_handle;
        native_handle m_parent;
    };
}
