/*  laplace/platform/basic_input.h
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

#ifndef laplace_platform_basic_input_h
#define laplace_platform_basic_input_h

#include "../core/input_handler.h"
#include "../core/slib.h"

namespace laplace::platform {
  class basic_input {
  public:
    static sl::time const default_char_predelay_msec;
    static sl::time const default_char_period_msec;

    struct cursor_state {
      sl::index x       = {};
      sl::index y       = {};
      sl::index delta_x = {};
      sl::index delta_y = {};
    };

    struct key_state {
      bool is_down = false;
    };

    static constexpr sl::whole key_count = 256;

    using keymap_table   = std::array<uint8_t, key_count>;
    using keyboard_state = std::array<key_state, key_count>;

    basic_input(basic_input const &) = delete;
    basic_input(basic_input &&)      = delete;
    auto operator=(basic_input const &) -> basic_input & = delete;
    auto operator=(basic_input &&) -> basic_input & = delete;

    basic_input() noexcept          = default;
    virtual ~basic_input() noexcept = default;

    [[nodiscard]] auto is_key_down(sl::index code) const noexcept
        -> bool;

    [[nodiscard]] auto is_key_up(sl::index code) const noexcept
        -> bool;

    [[nodiscard]] auto is_capslock() const noexcept -> bool;
    [[nodiscard]] auto is_numlock() const noexcept -> bool;
    [[nodiscard]] auto is_scrolllock() const noexcept -> bool;
    [[nodiscard]] auto is_alt() const noexcept -> bool;
    [[nodiscard]] auto is_shift() const noexcept -> bool;
    [[nodiscard]] auto is_control() const noexcept -> bool;

    [[nodiscard]] auto get_cursor_x() const noexcept -> sl::index;
    [[nodiscard]] auto get_cursor_y() const noexcept -> sl::index;

    [[nodiscard]] auto get_cursor_delta_x() const noexcept
        -> sl::index;

    [[nodiscard]] auto get_cursor_delta_y() const noexcept
        -> sl::index;

    [[nodiscard]] auto get_wheel_delta() const noexcept -> sl::index;

    [[nodiscard]] auto get_events() const noexcept
        -> std::span<const core::input_event>;

    void update_key(sl::index key, bool is_down) noexcept;
    void update_wheel(sl::index delta) noexcept;
    void update_cursor(sl::index x, sl::index y) noexcept;

    void set_char_predelay(sl::time predelay_msec) noexcept;
    void set_char_period(sl::time period_msec) noexcept;
    void set_keymap(keymap_table keymap) noexcept;
    void set_wheel_scale(sl::index scale) noexcept;

    void tick(sl::time delta_msec) noexcept;
    void refresh() noexcept;
    void reset_keyboard() noexcept;

    [[nodiscard]] auto map_key(sl::index key) noexcept -> uint8_t;
    
  private:
    [[nodiscard]] auto to_char(uint8_t key) noexcept -> char32_t;

    [[nodiscard]] auto process_char(uint8_t key,
                                    bool    is_down) noexcept
        -> char32_t;

    sl::vector<core::input_event> m_events;

    keymap_table   m_keymap   = {};
    keyboard_state m_keyboard = {};
    cursor_state   m_cursor;
    sl::index      m_wheel_delta = {};
    sl::index      m_wheel_scale = 1;

    sl::time m_char_predelay_msec = default_char_predelay_msec;
    sl::time m_char_period_msec   = default_char_period_msec;

    sl::time  m_char_time_msec  = 0;
    bool      m_is_char_pressed = false;
    sl::index m_last_char_key   = 0;
    char32_t  m_last_char       = U'\0';
  };
}

#endif
