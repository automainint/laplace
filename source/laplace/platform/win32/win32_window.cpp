/*  laplace/platform/win32/win32_window.cpp
 *
 *      Win32 window implementation.
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

#define laplace_windows_header_h
#include <windows.h>

#include "../../core/defs.h"
#include "window.h"
#include <chrono>
#include <iostream>

namespace laplace::win32 {
  using std::make_unique, std::string_view, std::wstring_view,
      std::shared_ptr, std::unique_lock, std::chrono::duration_cast,
      std::chrono::milliseconds;

  void window::init(window::native_handle parent) {
    if (!parent) {
      SetProcessDPIAware();
    }

    m_module_handle = GetModuleHandle(nullptr);

    m_handle = nullptr;
    m_parent = parent;

    get_exe_file_name();

    m_screen_width = static_cast<size_t>(GetSystemMetrics(SM_CXSCREEN));
    m_screen_height = static_cast<size_t>(GetSystemMetrics(SM_CYSCREEN));

    set_size(default_frame_width, default_frame_height);
    set_centered();

    create_window();
  }

  window::window() {
    init(nullptr);
  }

  window::window(window::native_handle parent) {
    init(parent);
  }

  window::~window() {
    destroy_window();
  }

  void window::on_init(event_init ev) {
    m_on_init = ev;
  }

  void window::on_cleanup(event_cleanup ev) {
    m_on_cleanup = ev;
  }

  void window::on_frame(event_frame ev) {
    m_on_frame = ev;
    ;
  }

  void window::on_size(event_size ev) {
    m_on_size = ev;
  }

  void window::on_focus(event_focus ev) {
    m_on_focus = ev;
  }

  void window::on_drop_file(event_drop_file ev) {
    m_on_drop_file = ev;
  }

  void window::set_name(string_view name) {
    m_window_name.clear();

    for (auto c : name) {
      m_window_name.append(1, static_cast<wchar_t>(c));
    }

    if (m_handle) {
      SetWindowTextW(m_handle, m_window_name.c_str());
    }
  }

  void window::set_name(wstring_view name) {
    m_window_name = name;

    if (m_handle) {
      SetWindowTextW(m_handle, name.data());
    }
  }

  void window::set_visible(bool is_visible) {
    m_is_visible = is_visible;

    if (m_handle) {
      update_window();

      ShowWindow(m_handle, is_visible ? SW_SHOW : SW_HIDE);
    }
  }

  void window::set_fullscreen(bool is_fullscreen) {
    if (m_is_fullscreen != is_fullscreen) {
      m_is_fullscreen = is_fullscreen;

      if (m_handle) {
        auto style    = static_cast<LONG>(get_style());
        auto style_ex = static_cast<LONG>(get_style_ex());

        SetWindowLong(m_handle, GWL_STYLE, style);
        SetWindowLong(m_handle, GWL_EXSTYLE, style_ex);

        update_window();
        update_display();
      }
    }
  }

  void window::set_centered() {
    m_x = (m_screen_width - m_width) >> 1;
    m_y = (m_screen_height - m_height) >> 1;

    update_rect();
  }

  void window::set_position(size_t x, size_t y) {
    m_x = x;
    m_y = y;

    update_rect();
  }

  void window::set_size(size_t frame_width, size_t frame_height) {
    if (frame_width > 0 && frame_height > 0) {
      m_frame_width  = frame_width;
      m_frame_height = frame_height;
    } else {
      m_frame_width  = default_frame_width;
      m_frame_height = default_frame_height;
    }

    adjust_window();
    update_rect();
  }

  void window::set_fullscreen_windowed() {
    set_fullscreen_mode(0, 0, m_frame_rate);
  }

  void window::set_fullscreen_mode(size_t frame_width,
                                   size_t frame_height,
                                   size_t frame_rate) {
    m_fullscreen_width  = frame_width;
    m_fullscreen_height = frame_height;
    m_frame_rate        = frame_rate;

    if (is_fullscreen()) {
      update_display();
    }
  }

  void window::set_input(shared_ptr<input> in) {
    m_input = in;

    if (m_input) {
      m_input->attach(m_handle);

      m_input->set_window_rect(
          get_x(), get_y(), get_width(), get_height());
    }
  }

  void window::set_parent(window::native_handle parent) {
    m_parent = parent;

    if (m_handle) {
      SetParent(m_handle, m_parent);
    }
  }

  void window::reset_parent() {
    m_parent = HWND_DESKTOP;

    if (m_handle) {
      SetParent(m_handle, HWND_DESKTOP);
    }
  }

  auto window::message_loop() -> int {
    using clock = std::chrono::steady_clock;

    if (!m_handle) {
      return -1;
    }

    if (m_is_visible) {
      ShowWindow(m_handle, SW_SHOWNORMAL);
      UpdateWindow(m_handle);
    }

    if (m_is_fullscreen) {
      update_display();
    }

    int  code = 0;
    bool quit = false;

    MSG msg;
    memset(&msg, 0, sizeof msg);

    if (m_on_init) {
      m_on_init();
    }

    m_is_inited = true;

    if (m_on_size) {
      m_on_size(get_frame_width(), get_frame_height());
    }

    auto time = clock::now();

    while (!quit) {
      while (PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
        if (GetMessageW(&msg, nullptr, 0, 0)) {
          TranslateMessage(&msg);
          DispatchMessageW(&msg);
        } else {
          quit = true;
        }

        if (msg.message == WM_QUIT) {
          code = static_cast<int>(msg.wParam);
          quit = true;
        }
      }

      auto delta = duration_cast<milliseconds>(clock::now() - time);
      auto delta_msec = static_cast<size_t>(delta.count());

      if (m_input) {
        m_input->tick(delta_msec);
      }

      if (m_on_frame) {
        m_on_frame(delta_msec);
      }

      if (m_input) {
        m_input->refresh();
      }

      time += delta;
    }

    if (m_on_cleanup) {
      m_on_cleanup();
    }

    destroy_window();

    return code;
  }

  void window::quit(int code) {
    PostQuitMessage(code);
  }

  void window::get_exe_file_name() {
    wchar_t chars[MAX_PATH + 1];
    auto    n = GetModuleFileNameW(m_module_handle, chars, MAX_PATH);
    chars[n]  = L'\0';

    m_exe_file_name = chars;
  }

  void window::load_icon(HICON &icon, HICON &icon_sm) {
    icon = LoadIcon(nullptr, IDI_APPLICATION);
  }

  auto window::register_class() -> bool {
    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof wcex);

    wcex.cbSize        = sizeof wcex;
    wcex.lpfnWndProc   = window_proc;
    wcex.hInstance     = m_module_handle;
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = m_class_name.c_str();

    load_icon(wcex.hIcon, wcex.hIconSm);

    if (!RegisterClassExW(&wcex)) {
      error(__FUNCTION__, "RegisterClassEx failed.");
      return false;
    }

    return true;
  }

  void window::create_window() {
    if (register_class()) {
      auto window_class = m_class_name.c_str();
      auto window_name  = m_window_name.c_str();

      m_handle = CreateWindowExW(
          static_cast<DWORD>(get_style_ex()), window_class, window_name,
          static_cast<DWORD>(get_style()), static_cast<int>(get_x()),
          static_cast<int>(get_y()), static_cast<int>(get_width()),
          static_cast<int>(get_height()), m_parent, nullptr,
          m_module_handle, nullptr);

      if (m_handle) {
        SetWindowLongPtr(m_handle, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(this));
      } else {
        error(__FUNCTION__, "CreateWindowEx failed.");
      }
    }
  }

  void window::adjust_window() {
    RECT rc { .left   = 0,
              .top    = 0,
              .right  = static_cast<LONG>(m_frame_width),
              .bottom = static_cast<LONG>(m_frame_height) };

    AdjustWindowRectEx(&rc, static_cast<DWORD>(get_style()), false,
                       static_cast<DWORD>(get_style_ex()));

    m_width  = static_cast<size_t>(static_cast<int64_t>(rc.right) -
                                  rc.left);
    m_height = static_cast<size_t>(static_cast<int64_t>(rc.bottom) -
                                   rc.top);
  }

  void window::update_rect() {
    if (m_handle) {
      size_t x      = get_x();
      size_t y      = get_y();
      size_t width  = get_width();
      size_t height = get_height();

      MoveWindow(m_handle, static_cast<int>(x), static_cast<int>(y),
                 static_cast<int>(width), static_cast<int>(height),
                 m_is_visible);

      if (m_input) {
        m_input->set_window_rect(x, y, width, height);
      }
    }
  }

  void window::update_window() {
    if (m_is_visible) {
      MoveWindow(m_handle, static_cast<int>(get_x()),
                 static_cast<int>(get_y()),
                 static_cast<int>(get_width()),
                 static_cast<int>(get_height()), true);
    }
  }

  void window::update_display() {
    bool toggle_on = (m_handle != nullptr) && is_fullscreen() &&
                     !is_fullscreen_windowed();

    if (toggle_on) {
      DEVMODE mode;
      memset(&mode, 0, sizeof mode);

      mode.dmSize   = sizeof mode;
      mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT |
                      DM_DISPLAYFREQUENCY;

      mode.dmBitsPerPel = 32;
      mode.dmPelsWidth  = static_cast<DWORD>(m_fullscreen_width);
      mode.dmPelsHeight = static_cast<DWORD>(m_fullscreen_height);
      mode.dmDisplayFrequency = static_cast<DWORD>(m_frame_rate);

      if (ChangeDisplaySettings(&mode, CDS_FULLSCREEN) !=
          DISP_CHANGE_SUCCESSFUL) {
        error(
            __FUNCTION__, "Toggle on. ChangeDisplaySettings failed.");
      }
    } else {
      /*  Reset to default settings.
       */

      if (ChangeDisplaySettings(nullptr, 0) != DISP_CHANGE_SUCCESSFUL) {
        error(__FUNCTION__,
              "Toggle off. ChangeDisplaySettings failed.");
      }
    }
  }

  void window::destroy_window() {
    if (m_handle) {
      DestroyWindow(m_handle);
      m_handle = nullptr;
    }

    if (is_fullscreen() && !is_fullscreen_windowed()) {
      update_display();
    }
  }

  void window::accept_files(void *drop_data) {
    auto drop = reinterpret_cast<HDROP>(drop_data);

    if (m_on_drop_file) {
      size_t count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);

      for (size_t i = 0; i < count; i++) {
        size_t length = DragQueryFileW(
            drop, static_cast<UINT>(i), nullptr, 0);
        auto file_name = make_unique<wchar_t[]>(length + 1);

        auto n = DragQueryFileW(drop, static_cast<UINT>(i),
                                file_name.get(),
                                static_cast<UINT>(length + 1));

        file_name.get()[n] = L'\0';
        m_on_drop_file(file_name.get());
      }
    }

    DragFinish(drop);
  }

  auto window::process(UINT message, WPARAM wparam, LPARAM lparam)
      -> LRESULT {
    switch (message) {
      case WM_INPUT:
        if (m_input && m_has_focus) {
          m_input->process(reinterpret_cast<HRAWINPUT>(lparam));
        }

        break;

      case WM_SIZE:
        if (m_on_size && m_is_inited) {
          auto width  = static_cast<size_t>(LOWORD(lparam));
          auto height = static_cast<size_t>(HIWORD(lparam));

          m_on_size(width, height);
        }
        break;

      case WM_ACTIVATE:
        switch (LOWORD(wparam)) {
          case WA_ACTIVE:
          case WA_CLICKACTIVE: m_has_focus = true; break;

          case WA_INACTIVE:
            if (m_input)
              m_input->reset();

            m_has_focus = false;
            break;
        }

        if (m_on_focus) {
          m_on_focus(m_has_focus);
        }

        break;

      case WM_DROPFILES:
        accept_files(reinterpret_cast<HDROP>(wparam));
        break;

      case WM_CLOSE: PostQuitMessage(0); break;

      default:
        return DefWindowProcW(m_handle, message, wparam, lparam);
    }

    return 0;
  }

  auto CALLBACK window::window_proc(HWND window_handle, UINT message,
                                    WPARAM wparam, LPARAM lparam)
      -> LRESULT {
    if (message != WM_CREATE) {
      window *w = reinterpret_cast<window *>(
          GetWindowLongPtr(window_handle, GWLP_USERDATA));

      if (w) {
        return w->process(message, wparam, lparam);
      } else {
        return DefWindowProcW(window_handle, message, wparam, lparam);
      }
    }

    return 0;
  }

  auto window::get_screen_width() const -> size_t {
    return m_screen_width;
  }

  auto window::get_screen_height() const -> size_t {
    return m_screen_height;
  }

  auto window::get_fullscreen_width() const -> size_t {
    return m_fullscreen_width == 0 ? get_screen_width()
                                   : m_fullscreen_width;
  }

  auto window::get_fullscreen_height() const -> size_t {
    return m_fullscreen_height == 0 ? get_screen_height()
                                    : m_fullscreen_height;
  }

  auto window::get_style_ex() const -> size_t {
    auto style_ex = m_style_ex;

    if (m_is_fullscreen) {
      style_ex &= ~WS_EX_ACCEPTFILES;
    }

    return style_ex;
  }

  auto window::get_style() const -> size_t {
    auto style = m_style;

    if (m_is_fullscreen) {
      style |= WS_POPUP;
      style &= mask_fullscreen_style;
    }

    return style;
  }

  auto window::get_x() const -> size_t {
    return m_is_fullscreen ? m_screen_x : m_x;
  }

  auto window::get_y() const -> size_t {
    return m_is_fullscreen ? m_screen_y : m_y;
  }

  auto window::get_width() const -> size_t {
    return m_is_fullscreen ? get_fullscreen_width() : m_width;
  }

  auto window::get_height() const -> size_t {
    return m_is_fullscreen ? get_fullscreen_height() : m_height;
  }

  auto window::get_frame_width() const -> size_t {
    return m_frame_width;
  }

  auto window::get_frame_height() const -> size_t {
    return m_frame_height;
  }

  auto window::is_visible() const -> bool {
    return m_is_visible;
  }

  auto window::is_fullscreen() const -> bool {
    return m_is_fullscreen;
  }

  auto window::is_fullscreen_windowed() const -> bool {
    return m_fullscreen_width == 0 || m_fullscreen_height == 0;
  }

  auto window::has_focus() const -> bool {
    return m_has_focus;
  }

  auto window::has_cursor() const -> bool {
    return m_has_cursor;
  }

  auto window::get_native_handle() const -> HWND {
    return m_handle;
  }
}
