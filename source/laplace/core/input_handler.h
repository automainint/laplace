/*  laplace/core/input_handler.h
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

#ifndef laplace_core_input_handler_h
#define laplace_core_input_handler_h

#include "defs.h"
#include "keys.h"
#include <functional>

namespace laplace::core {
  struct input_event {
    static constexpr sl::index delta_key_down = 1;
    static constexpr sl::index delta_key_up   = -1;

    bool is_capslock   = false;
    bool is_numlock    = false;
    bool is_scrolllock = false;
    bool is_alt        = false;
    bool is_shift      = false;
    bool is_control    = false;

    sl::index cursor_x  = {};
    sl::index cursor_y  = {};
    sl::index key       = {};
    sl::index delta     = {};
    char32_t  character = {};
  };

  constexpr auto is_key_down(input_event const &ev) noexcept -> bool;
  constexpr auto is_key_up(input_event const &ev) noexcept -> bool;

  constexpr auto get_wheel_delta(input_event const &ev) noexcept
      -> sl::index;

  using fn_get_bool_state = std::function<bool()>;
  using fn_get_int_state  = std::function<sl::index()>;
  using fn_get_key_state  = std::function<bool(sl::index)>;

  using fn_get_events = std::function<std::span<const input_event>()>;

  struct input_handler {
    fn_get_bool_state is_capslock;
    fn_get_bool_state is_numlock;
    fn_get_bool_state is_scrolllock;
    fn_get_bool_state is_alt;
    fn_get_bool_state is_shift;
    fn_get_bool_state is_control;

    fn_get_key_state is_key_down;
    fn_get_key_state is_key_up;

    fn_get_int_state get_mouse_resolution_x;
    fn_get_int_state get_mouse_resolution_y;
    fn_get_int_state get_mouse_x;
    fn_get_int_state get_mouse_y;
    fn_get_int_state get_mouse_delta_x;
    fn_get_int_state get_mouse_delta_y;
    fn_get_int_state get_cursor_x;
    fn_get_int_state get_cursor_y;
    fn_get_int_state get_wheel_delta;

    fn_get_events get_events;
  };

  using cref_input_handler = input_handler const &;
}

#include "input_handler.impl.h"

#endif
