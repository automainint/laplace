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
#include <functional>

namespace laplace::core {
  using fn_get_bool_state = std::function<bool()>;
  using fn_get_int_state  = std::function<sl::index()>;
  using fn_get_key_state  = std::function<bool(sl::index)>;
  using fn_get_text       = std::function<std::u8string_view()>;

  struct input_handler {
    fn_get_bool_state is_capslock;
    fn_get_bool_state is_numlock;
    fn_get_bool_state is_scrolllock;
    fn_get_bool_state is_alt;
    fn_get_bool_state is_shift;
    fn_get_bool_state is_control;

    fn_get_key_state is_key_down;
    fn_get_key_state is_key_up;
    fn_get_key_state is_key_changed;
    fn_get_key_state is_key_pressed;
    fn_get_key_state is_key_unpressed;

    fn_get_int_state get_mouse_resolution_x;
    fn_get_int_state get_mouse_resolution_y;
    fn_get_int_state get_mouse_x;
    fn_get_int_state get_mouse_y;
    fn_get_int_state get_mouse_delta_x;
    fn_get_int_state get_mouse_delta_y;
    fn_get_int_state get_cursor_x;
    fn_get_int_state get_cursor_y;
    fn_get_int_state get_wheel_delta;

    fn_get_text get_text;
  };

  using cref_input_handler = const input_handler &;
}

#endif
