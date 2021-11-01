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

#include "basic_input.h"
#include "events.h"
#include "opengl.h"
#include "thread.h"
#include <cstdint>
#include <string>

namespace laplace::platform::dummy {
  inline void set_background_mode(bool is_background_mode) noexcept {
  }

  inline void set_realtime_mode(bool is_realtime_mode) noexcept { }

  inline void set_thread_priority(int priority) noexcept { }

  inline void set_thread_priority(std::thread &th,
                                  int          priority) noexcept { }

  [[nodiscard]] inline auto gl_get_proc_address(
      char const *s) noexcept -> gl::ptr_function {
    return nullptr;
  }

  class input : public basic_input {
  public:
    ~input() noexcept override = default;

    void use_system_cursor(bool) noexcept { }
    void set_cursor_enabled(bool) noexcept { }
    void set_mouse_resolution(sl::whole, sl::whole) noexcept { }
    void set_clamp(bool, bool) noexcept { }

    [[nodiscard]] auto get_mouse_resolution_x() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_mouse_resolution_y() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_mouse_x() const noexcept -> sl::index {
      return 0;
    }

    [[nodiscard]] auto get_mouse_y() const noexcept -> sl::index {
      return 0;
    }

    [[nodiscard]] auto get_mouse_delta_x() const noexcept
        -> sl::index {
      return 0;
    }

    [[nodiscard]] auto get_mouse_delta_y() const noexcept
        -> sl::index {
      return 0;
    }
  };

  class window {
  public:
    using native_handle = std::nullptr_t;

    static constexpr sl::whole default_frame_width  = 0;
    static constexpr sl::whole default_frame_height = 0;
    static constexpr sl::whole default_frame_rate   = 0;

    window(window const &) = delete;
    window(window &&)      = delete;
    auto operator=(window const &) -> window & = delete;
    auto operator=(window &&) -> window & = delete;

    window() noexcept = default;
    explicit window(native_handle) noexcept { }
    ~window() noexcept = default;

    void on_init(event_init) noexcept { }
    void on_cleanup(event_cleanup) noexcept { }
    void on_frame(event_frame) noexcept { }
    void on_size(event_size) noexcept { }
    void on_focus(event_focus) noexcept { }
    void on_drop_file(event_drop_file) noexcept { }

    void set_name(std::string_view) noexcept { }
    void set_name(std::wstring_view) noexcept { }

    void set_visible(bool) noexcept { }
    void set_fullscreen(bool) noexcept { }

    void set_centered() noexcept { }
    void set_position(sl::index, sl::index) noexcept { }
    void set_size(sl::whole, sl::whole) noexcept { }

    void set_fullscreen_windowed() noexcept { }
    void set_fullscreen_mode(sl::whole,
                             sl::whole,
                             sl::whole) noexcept { }

    void set_input(std::shared_ptr<input>) noexcept { }

    void set_parent(native_handle) noexcept { }
    void reset_parent() noexcept { }

    auto message_loop() noexcept -> int {
      return 0;
    }

    void quit(int = 0) noexcept { }

    [[nodiscard]] auto get_screen_width() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_screen_height() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_fullscreen_width() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_fullscreen_height() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_x() const noexcept -> sl::index {
      return 0u;
    }

    [[nodiscard]] auto get_y() const noexcept -> sl::index {
      return 0u;
    }

    [[nodiscard]] auto get_width() const noexcept -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_height() const noexcept -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_frame_width() const noexcept -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto get_frame_height() const noexcept
        -> sl::whole {
      return 0u;
    }

    [[nodiscard]] auto is_visible() const noexcept -> bool {
      return false;
    }

    [[nodiscard]] auto is_fullscreen() const noexcept -> bool {
      return false;
    }

    [[nodiscard]] auto is_fullscreen_windowed() const noexcept
        -> bool {
      return false;
    }

    [[nodiscard]] auto has_focus() const noexcept -> bool {
      return false;
    }

    [[nodiscard]] auto has_cursor() const noexcept -> bool {
      return false;
    }

    [[nodiscard]] auto get_native_handle() const noexcept
        -> native_handle {
      return nullptr;
    }
  };

  class glcontext {
  public:
    glcontext(glcontext const &) = delete;
    glcontext(glcontext &&)      = delete;
    auto operator=(glcontext const &) -> glcontext & = delete;
    auto operator=(glcontext &&) -> glcontext & = delete;

    explicit glcontext(std::shared_ptr<window>) noexcept { }
    ~glcontext() noexcept = default;

    void        swap_buffers() noexcept { }
    static void set_forward_compatible(bool) noexcept { }
  };

  class audio { };
  class clipboard { };
}

#endif
