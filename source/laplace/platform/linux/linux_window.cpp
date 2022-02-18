/*  laplace/platform/linux/linux_window.cpp
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

#include "window.h"

#include "../../core/string.h"
#include "glx.h"
#include <chrono>

namespace laplace::linux {
  using std::chrono::steady_clock, std::chrono::milliseconds,
      std::chrono::duration_cast;

  sl::vector<window *> window::windows;

  char const      window::default_name[]      = "Laplace Application";
  bool const      window::default_is_visible  = true;
  sl::whole const window::default_frame_width = 960;
  sl::whole const window::default_frame_height = 720;
  sl::whole const window::default_frame_rate   = 24;

  sl::whole window::xevent_mask = KeyPressMask | KeyReleaseMask |
                                  ButtonPressMask |
                                  ButtonReleaseMask |
                                  EnterWindowMask | LeaveWindowMask |
                                  PointerMotionMask |
                                  VisibilityChangeMask |
                                  FocusChangeMask |
                                  StructureNotifyMask |
                                  SubstructureNotifyMask;

  window::window() noexcept {
    init();
  }

  window::window(native_handle) noexcept {
    init();
  }

  window::~window() noexcept {
    cleanup();
  }

  void window::on_init(event_init ev) noexcept {
    m_on_init = std::move(ev);
  }

  void window::on_cleanup(event_cleanup ev) noexcept {
    m_on_cleanup = std::move(ev);
  }

  void window::on_frame(event_frame ev) noexcept {
    m_on_frame = std::move(ev);
  }

  void window::on_size(event_size ev) noexcept {
    m_on_size = std::move(ev);
  }

  void window::on_focus(event_focus ev) noexcept {
    m_on_focus = std::move(ev);
  }

  void window::on_drop_file(event_drop_file ev) noexcept {
    m_on_drop_file = std::move(ev);
  }

  void window::set_name(std::string_view name) noexcept {
    m_name = name;

    if (m_ok) {
      XStoreName(m_display, m_handle, m_name.c_str());
    }
  }

  void window::set_name(std::wstring_view name) noexcept {
    set_name(to_string(name));
  }

  void window::set_visible(bool is_visible) noexcept {
    if (m_is_visible != is_visible) {
      m_is_visible = is_visible;

      if (m_ok) {
        if (is_visible)
          XMapWindow(m_display, m_handle);
        else
          XUnmapWindow(m_display, m_handle);
      }
    }
  }

  void window::set_fullscreen(bool) noexcept { }

  void window::set_centered() noexcept { }

  void window::set_position(sl::index, sl::index) noexcept { }

  void window::set_size(sl::whole frame_width,
                        sl::whole frame_height) noexcept {
    m_width  = frame_width;
    m_height = frame_height;
  }

  void window::set_fullscreen_windowed() noexcept { }

  void window::set_fullscreen_mode(sl::whole frame_width,
                                   sl::whole frame_height,
                                   sl::whole frame_rate) noexcept { }

  void window::set_input(std::shared_ptr<input> in) noexcept {
    m_input = std::move(in);

    if (m_display != nullptr)
      m_input->init_keymap(m_display);
  }

  void window::set_parent(native_handle) noexcept { }

  void window::reset_parent() noexcept { }

  auto window::message_loop() noexcept -> int {
    using clock = steady_clock;

    if (!m_ok)
      return 0;

    auto wm_delete = XInternAtom(m_display, "WM_DELETE_WINDOW",
                                 False);

    XSetWMProtocols(m_display, m_handle, &wm_delete, 1);

    auto ev                = XEvent {};
    auto window_attributes = XWindowAttributes {};

    if (m_on_init)
      m_on_init();

    if (m_on_size)
      m_on_size(get_frame_width(), get_frame_height());

    auto time = clock::now();

    while (!m_done) {
      sl::index count = XEventsQueued(m_display, QueuedAlready);

      if (count == 0)
        XFlush(m_display);

      while (count-- > 0) {
        XNextEvent(m_display, &ev);

        switch (ev.type) {
          case DestroyNotify: m_done = true; break;

          case MotionNotify:
            if (m_input)
              m_input->update_cursor(ev.xmotion.x, ev.xmotion.y);
            break;

          case ButtonPress:
            if (m_input) {
              m_input->update_cursor(ev.xbutton.x, ev.xbutton.y);

              if (ev.xbutton.button <= Button3) {
                m_input->update_button(ev.xbutton.button, true);
              } else if (ev.xbutton.button == Button4) {
                m_input->update_wheel(1);
              } else if (ev.xbutton.button == Button5) {
                m_input->update_wheel(-1);
              }
            }
            break;

          case ButtonRelease:
            if (m_input) {
              m_input->update_cursor(ev.xbutton.x, ev.xbutton.y);

              if (ev.xbutton.button <= Button3)
                m_input->update_button(ev.xbutton.button, false);
            }
            break;

          case KeyPress:
            if (m_input) {
              m_input->update_cursor(ev.xkey.x, ev.xkey.y);
              m_input->update_controls(ev.xkey.keycode, true);
              m_input->update_key(ev.xkey.keycode, true);
            }
            break;

          case KeyRelease:
            if (m_input) {
              m_input->update_cursor(ev.xkey.x, ev.xkey.y);
              m_input->update_controls(ev.xkey.keycode, false);
              m_input->update_key(ev.xkey.keycode, false);
            }
            break;

          case EnterNotify: m_has_cursor = true; break;
          case LeaveNotify: m_has_cursor = false; break;

          case FocusIn:
            m_has_focus = true;
            if (m_on_focus)
              m_on_focus(true);
            break;

          case FocusOut:
            m_has_focus = false;
            if (m_on_focus)
              m_on_focus(false);
            if (m_input)
              m_input->reset_keyboard();
            break;

          case ClientMessage:
            if (ev.xclient.data.l[0] == wm_delete)
              m_done = true;
            break;
        }
      }

      XGetWindowAttributes(m_display, m_handle, &window_attributes);

      update_position(window_attributes.x, window_attributes.y);

      update_frame_size(window_attributes.width,
                        window_attributes.height);

      auto delta = duration_cast<milliseconds>(clock::now() - time);
      auto delta_msec = static_cast<sl::whole>(delta.count());

      if (m_input)
        m_input->tick(delta_msec);

      if (m_on_frame)
        m_on_frame(delta_msec);

      if (m_input)
        m_input->refresh();

      time += delta;
    }

    if (m_on_cleanup)
      m_on_cleanup();

    return m_code;
  }

  void window::quit(int code) noexcept {
    m_code = code;
    m_done = true;
  }

  auto window::get_screen_width() const noexcept -> sl::whole {
    return 0;
  }

  auto window::get_screen_height() const noexcept -> sl::whole {
    return 0;
  }

  auto window::get_fullscreen_width() const noexcept -> sl::whole {
    return 0;
  }

  auto window::get_fullscreen_height() const noexcept -> sl::whole {
    return 0;
  }

  auto window::get_x() const noexcept -> sl::index {
    return m_x;
  }

  auto window::get_y() const noexcept -> sl::index {
    return m_y;
  }

  auto window::get_width() const noexcept -> sl::whole {
    return m_width;
  }

  auto window::get_height() const noexcept -> sl::whole {
    return m_height;
  }

  auto window::get_frame_width() const noexcept -> sl::whole {
    return m_frame_width;
  }

  auto window::get_frame_height() const noexcept -> sl::whole {
    return m_frame_height;
  }

  auto window::is_visible() const noexcept -> bool {
    return m_is_visible;
  }

  auto window::is_fullscreen() const noexcept -> bool {
    return false;
  }

  auto window::is_fullscreen_windowed() const noexcept -> bool {
    return false;
  }

  auto window::has_focus() const noexcept -> bool {
    return m_has_focus;
  }

  auto window::has_cursor() const noexcept -> bool {
    return m_has_cursor;
  }

  auto window::get_display() const noexcept -> Display * {
    return m_display;
  }

  auto window::get_native_handle() const noexcept -> native_handle {
    return m_handle;
  }

  auto window::xlib_error(Display     *display,
                          XErrorEvent *error) noexcept -> int {
    for (auto *win : windows) {
      if (win->m_display != display)
        continue;

      if (error->type == BadWindow) {
        win->m_done = true;
        win->m_ok   = false;
      }

      break;
    }

    return 0;
  }

  void window::init() noexcept {
    if (!gl_load_library(log))
      return;

    windows.emplace_back(this);

    XSetErrorHandler(xlib_error);

    m_display = XOpenDisplay(nullptr);

    if (m_display == nullptr) {
      log(log_event::error, "XOpenDisplay failed.", __FUNCTION__);
      return;
    }

    auto root = DefaultRootWindow(m_display);

    int attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24,
                         GLX_DOUBLEBUFFER, 0 };

    auto *visual = glXChooseVisual(m_display, 0, attributes);

    if (visual == nullptr) {
      log(log_event::error, "glXChooseVisual failed.", __FUNCTION__);
      return;
    }

    auto colormap = XCreateColormap(m_display, root, visual->visual,
                                    AllocNone);

    auto window_attributes = XSetWindowAttributes {
      .event_mask = xevent_mask, .colormap = colormap
    };

    m_handle = XCreateWindow(
        m_display, root, get_x(), get_y(), get_width(), get_height(),
        0, visual->depth, InputOutput, visual->visual,
        CWColormap | CWEventMask, &window_attributes);

    XStoreName(m_display, m_handle, m_name.c_str());

    if (is_visible())
      XMapWindow(m_display, m_handle);

    m_context = glXCreateContext(m_display, visual, nullptr, 1);

    if (m_context == nullptr) {
      log(log_event::error, "glXCreateContext failed.", __FUNCTION__);
      XDestroyWindow(m_display, m_handle);
      return;
    }

    glXMakeCurrent(m_display, m_handle, m_context);

    if (m_input)
      m_input->init_keymap(m_display);

    m_ok = true;
  }

  void window::cleanup() noexcept {
    if (m_context != nullptr) {
      glXMakeCurrent(m_display, 0, nullptr);
      glXDestroyContext(m_display, m_context);
      m_context = nullptr;
    }

    if (m_ok)
      XDestroyWindow(m_display, m_handle);

    if (m_display != nullptr) {
      XCloseDisplay(m_display);
      m_display = nullptr;
    }

    gl_free_library();

    if (auto i = std::find(windows.begin(), windows.end(), this);
        i != windows.end())
      windows.erase(i);

    m_ok = false;
  }

  void window::update_position(sl::index x, sl::index y) noexcept {
    m_x = x;
    m_y = y;
  }

  void window::update_frame_size(sl::whole width,
                                 sl::whole height) noexcept {
    if (m_frame_width == width && m_frame_height == height)
      return;

    m_frame_width  = width;
    m_frame_height = height;

    if (m_on_size)
      m_on_size(width, height);
  }
}
