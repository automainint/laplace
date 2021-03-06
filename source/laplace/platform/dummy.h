/*  laplace/platform/dummy.h
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

#ifndef laplace_platform_dummy_h
#define laplace_platform_dummy_h

#include "events.h"
#include <cstdint>
#include <string>

namespace laplace::platform::dummy {
  auto gl_init() -> bool {
    return false;
  }

  void gl_cleanup() { }

  class input {
  public:
    input() { }
    ~input() { }

    void on_key_down(event_key_down) { }
    void on_wheel(event_wheel) { }

    void use_system_cursor(bool) { }
    void set_cursor_enabled(bool) { }
    void set_mouse_resolution(size_t, size_t) { }
    void set_clamp(bool, bool) { }

    auto is_capslock() const -> bool {
      return false;
    }

    auto is_numlock() const -> bool {
      return false;
    }

    auto is_scrolllock() const -> bool {
      return false;
    }

    auto is_alt() const -> bool {
      return false;
    }

    auto is_shift() const -> bool {
      return false;
    }

    auto is_control() const -> bool {
      return false;
    }

    auto is_key_down(int) const -> bool {
      return false;
    }

    auto is_key_up(int) const -> bool {
      return true;
    }

    auto is_key_changed(int) const -> bool {
      return false;
    }

    auto is_key_pressed(int) const -> bool {
      return false;
    }

    auto is_key_unpressed(int) const -> bool {
      return false;
    }

    auto get_mouse_resolution_x() const -> size_t {
      return 0u;
    }

    auto get_mouse_resolution_y() const -> size_t {
      return 0u;
    }

    auto get_mouse_x() const -> int {
      return 0;
    }

    auto get_mouse_y() const -> int {
      return 0;
    }

    auto get_mouse_delta_x() const -> int {
      return 0;
    }

    auto get_mouse_delta_y() const -> int {
      return 0;
    }

    auto get_cursor_x() const -> int {
      return 0;
    }

    auto get_cursor_y() const -> int {
      return 0;
    }

    auto get_wheel_delta() const -> int {
      return 0;
    }

    auto get_text() const -> std::u8string_view {
      static constexpr auto nil = u8"";
      return nil;
    }

    void refresh() { }
  };

  class window {
  public:
    using native_handle = std::nullptr_t;

    window() { }
    window(native_handle) { }

    void on_init(event_init) { }
    void on_cleanup(event_cleanup) { }
    void on_frame(event_frame) { }
    void on_size(event_size) { }
    void on_focus(event_focus) { }
    void on_drop_file(event_drop_file) { }

    void set_name(std::string_view) { }
    void set_name(std::wstring_view) { }

    void set_visible(bool) { }
    void set_fullscreen(bool) { }

    void set_centered() { }
    void set_position(size_t, size_t) { }
    void set_size(size_t, size_t) { }

    void set_fullscreen_windowed() { }
    void set_fullscreen_mode(size_t, size_t, size_t) { }

    void set_input(std::shared_ptr<input>) { }

    void set_parent(native_handle) { }
    void reset_parent() { }

    auto message_loop() -> int { }
    void quit(int = 0) { }

    auto get_screen_width() const -> size_t {
      return 0u;
    }

    auto get_screen_height() const -> size_t {
      return 0u;
    }

    auto get_fullscreen_width() const -> size_t {
      return 0u;
    }

    auto get_fullscreen_height() const -> size_t {
      return 0u;
    }

    auto get_x() const -> size_t {
      return 0u;
    }

    auto get_y() const -> size_t {
      return 0u;
    }

    auto get_width() const -> size_t {
      return 0u;
    }

    auto get_height() const -> size_t {
      return 0u;
    }

    auto get_frame_width() const -> size_t {
      return 0u;
    }

    auto get_frame_height() const -> size_t {
      return 0u;
    }

    auto is_visible() const -> bool {
      return false;
    }

    auto is_fullscreen() const -> bool {
      return false;
    }

    auto is_fullscreen_windowed() const -> bool {
      return false;
    }

    auto has_focus() const -> bool {
      return false;
    }

    auto has_cursor() const -> bool {
      return false;
    }

    auto get_native_handle() const -> native_handle {
      return nullptr;
    }
  };

  class glcontext {
  public:
    glcontext(std::shared_ptr<window>) { }
    void        swap_buffers() { }
    static void set_forward_compatible(bool) { }
  };

  class audio { };
  class clipboard { };
}

#endif
