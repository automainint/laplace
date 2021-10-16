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

#ifndef laplace_platform_win32_window_h
#define laplace_platform_win32_window_h

#include "../../core/options.h"
#include "../events.h"
#include "input.h"
#include "win.predef.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace laplace::win32 {
  class window {
  public:
    using native_handle = HWND;

    static const wchar_t   class_name[];
    static const wchar_t   default_window_name[];
    static const bool      default_is_visible;
    static const bool      default_is_fullscreen;
    static const sl::whole default_frame_width;
    static const sl::whole default_frame_height;
    static const sl::whole default_frame_rate;

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
    void set_position(sl::index x, sl::index y);
    void set_size(sl::whole frame_width, sl::whole frame_height);

    void set_fullscreen_windowed();
    void set_fullscreen_mode(sl::whole frame_width,
                             sl::whole frame_height,
                             sl::whole frame_frequency);

    void set_input(std::shared_ptr<input> in);

    void set_parent(native_handle parent);
    void reset_parent();

    auto message_loop() -> int;
    void quit(int code = 0);

    auto get_screen_width() const -> sl::whole;
    auto get_screen_height() const -> sl::whole;

    auto get_fullscreen_width() const -> sl::whole;
    auto get_fullscreen_height() const -> sl::whole;

    auto get_x() const -> sl::index;
    auto get_y() const -> sl::index;

    auto get_width() const -> sl::whole;
    auto get_height() const -> sl::whole;

    auto get_frame_width() const -> sl::whole;
    auto get_frame_height() const -> sl::whole;

    auto is_visible() const -> bool;
    auto is_fullscreen() const -> bool;

    auto is_fullscreen_windowed() const -> bool;

    auto has_focus() const -> bool;
    auto has_cursor() const -> bool;

    auto get_native_handle() const -> native_handle;

  private:
    void init(native_handle parent);
    auto get_style_ex() const -> sl::whole;
    auto get_style() const -> sl::whole;
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
    auto process(UINT message, WPARAM wparam, LPARAM lparam)
        -> LRESULT;

    static auto __stdcall window_proc(HWND   window_handle,
                                      UINT   message,
                                      WPARAM wparam,
                                      LPARAM lparam) -> LRESULT;

    static const uint32_t default_style_ex;
    static const uint32_t default_style;
    static const uint32_t mask_fullscreen_style;

    std::shared_ptr<input> m_input;

    std::wstring m_exe_file_name;
    std::wstring m_class_name  = class_name;
    std::wstring m_window_name = default_window_name;

    sl::whole m_style_ex = default_style_ex;
    sl::whole m_style    = default_style;

    event_init      m_on_init;
    event_cleanup   m_on_cleanup;
    event_frame     m_on_frame;
    event_size      m_on_size;
    event_focus     m_on_focus;
    event_drop_file m_on_drop_file;

    sl::index m_screen_x               = 0;
    sl::index m_screen_y               = 0;
    sl::whole m_screen_width           = 0;
    sl::whole m_screen_height          = 0;
    sl::whole m_fullscreen_width       = 0;
    sl::whole m_fullscreen_height      = 0;
    sl::index m_x                      = 0;
    sl::index m_y                      = 0;
    sl::whole m_width                  = 0;
    sl::whole m_height                 = 0;
    sl::whole m_frame_width            = 0;
    sl::whole m_frame_height           = 0;
    sl::whole m_init_fullscreen_width  = default_frame_width;
    sl::whole m_init_fullscreen_height = default_frame_height;
    sl::whole m_init_frame_rate        = default_frame_rate;

    bool m_is_inited     = false;
    bool m_is_visible    = default_is_visible;
    bool m_is_fullscreen = default_is_fullscreen;
    bool m_has_focus     = false;
    bool m_has_cursor    = false;

    HINSTANCE     m_module_handle = nullptr;
    native_handle m_handle        = nullptr;
    native_handle m_parent        = nullptr;
  };
}

#endif
