/*  laplace/platform/linux/window.h
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

#ifndef laplace_platform_linux_window_h
#define laplace_platform_linux_window_h

#include "../events.h"
#include "input.h"

#include "glx.h"

namespace laplace::linux {
  class window {
  public:
    using native_handle = Window;

    static char const      default_name[];
    static bool const      default_is_visible;
    static sl::whole const default_frame_width;
    static sl::whole const default_frame_height;
    static sl::whole const default_frame_rate;

    window(window const &) = delete;
    window(window &&)      = delete;
    auto operator=(window const &) -> window & = delete;
    auto operator=(window &&) -> window & = delete;

    window() noexcept;
    explicit window(native_handle) noexcept;
    ~window() noexcept;

    void on_init(event_init) noexcept;
    void on_cleanup(event_cleanup) noexcept;
    void on_frame(event_frame) noexcept;
    void on_size(event_size) noexcept;
    void on_focus(event_focus) noexcept;
    void on_drop_file(event_drop_file) noexcept;

    void set_name(std::string_view) noexcept;
    void set_name(std::wstring_view) noexcept;

    void set_visible(bool) noexcept;
    void set_fullscreen(bool) noexcept;

    void set_centered() noexcept;
    void set_position(sl::index, sl::index) noexcept;
    void set_size(sl::whole, sl::whole) noexcept;

    void set_fullscreen_windowed() noexcept;
    void set_fullscreen_mode(sl::whole,
                             sl::whole,
                             sl::whole) noexcept;

    void set_input(std::shared_ptr<input> in) noexcept;

    void set_parent(native_handle) noexcept;
    void reset_parent() noexcept;

    auto message_loop() noexcept -> int;

    void quit(int code = 0) noexcept;

    auto get_screen_width() const noexcept -> sl::whole;

    auto get_screen_height() const noexcept -> sl::whole;

    auto get_fullscreen_width() const noexcept -> sl::whole;

    auto get_fullscreen_height() const noexcept -> sl::whole;

    auto get_x() const noexcept -> sl::index;

    auto get_y() const noexcept -> sl::index;

    auto get_width() const noexcept -> sl::whole;

    auto get_height() const noexcept -> sl::whole;

    auto get_frame_width() const noexcept -> sl::whole;

    auto get_frame_height() const noexcept -> sl::whole;

    auto is_visible() const noexcept -> bool;

    auto is_fullscreen() const noexcept -> bool;

    auto is_fullscreen_windowed() const noexcept -> bool;

    auto has_focus() const noexcept -> bool;

    auto has_cursor() const noexcept -> bool;

    auto get_display() const noexcept -> Display *;
    auto get_native_handle() const noexcept -> native_handle;

  private:
    static sl::vector<window *> windows;

    static auto xlib_error(Display     *display,
                           XErrorEvent *error) noexcept -> int;

    void init() noexcept;
    void cleanup() noexcept;

    void update_position(sl::index x, sl::index y) noexcept;

    void update_frame_size(sl::whole width,
                           sl::whole height) noexcept;

    static sl::whole xevent_mask;

    event_init      m_on_init;
    event_cleanup   m_on_cleanup;
    event_frame     m_on_frame;
    event_size      m_on_size;
    event_focus     m_on_focus;
    event_drop_file m_on_drop_file;

    bool          m_ok         = false;
    bool          m_is_visible = default_is_visible;
    bool          m_has_cursor = false;
    bool          m_has_focus  = false;
    bool          m_done       = false;
    int           m_code       = 0;
    Display      *m_display    = nullptr;
    GLXContext    m_context    = nullptr;
    native_handle m_handle     = {};

    std::string m_name         = default_name;
    sl::whole   m_x            = 0;
    sl::whole   m_y            = 0;
    sl::whole   m_width        = default_frame_width;
    sl::whole   m_height       = default_frame_height;
    sl::whole   m_frame_width  = 0;
    sl::whole   m_frame_height = 0;

    std::shared_ptr<input> m_input;
  };
}

#endif
