/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "input.h"

namespace laplace::linux {
  using namespace core::keys;
  using std::array;

  sl::index const input::default_wheel_scale = 120;

  input::input() noexcept {
    set_wheel_scale(default_wheel_scale);
  }

  void input::use_system_cursor(bool) noexcept { }
  void input::set_cursor_enabled(bool) noexcept { }
  void input::set_mouse_resolution(sl::whole, sl::whole) noexcept { }
  void input::set_clamp(bool, bool) noexcept { }

  auto input::get_mouse_resolution_x() const noexcept -> sl::whole {
    return 0;
  }

  auto input::get_mouse_resolution_y() const noexcept -> sl::whole {
    return 0;
  }

  auto input::get_mouse_x() const noexcept -> sl::index {
    return get_cursor_x();
  }

  auto input::get_mouse_y() const noexcept -> sl::index {
    return get_cursor_y();
  }

  auto input::get_mouse_delta_x() const noexcept -> sl::index {
    return get_cursor_delta_x();
  }

  auto input::get_mouse_delta_y() const noexcept -> sl::index {
    return get_cursor_delta_y();
  }

  void input::update_button(sl::index button, bool is_down) noexcept {
    if (button == Button1)
      update_key(in_lbutton, is_down);
    if (button == Button2)
      update_key(in_mbutton, is_down);
    if (button == Button3)
      update_key(in_rbutton, is_down);
  }

  void input::update_controls(sl::index key, bool is_down) noexcept {
    auto const code = map_key(key);

    switch (code) {
      case key_lshift:
      case key_rshift: update_key(in_shift, is_down); break;

      case key_lcontrol:
      case key_rcontrol: update_key(in_control, is_down); break;

      case key_lalt:
      case key_ralt: update_key(in_alt, is_down); break;

      case key_capslock_toggle:
        toggle(in_capslock, is_down, m_toggle_capslock);
        break;

      case key_numlock_toggle:
        toggle(in_numlock, is_down, m_toggle_numlock);
        break;

      case key_scrolllock_toggle:
        toggle(in_scrolllock, is_down, m_toggle_scrolllock);
        break;

      default:;
    }
  }

  void input::init_keymap(Display *display) noexcept {
    adjust_and_set_keymap(load_keymap(display));
  }

  auto input::load_keymap(Display *display) noexcept -> keymap_table {
    auto v = keymap_table {};

    auto const code = [&](KeySym k) {
      return XKeysymToKeycode(display, k);
    };

    v[code(XK_Left)]  = key_left;
    v[code(XK_Right)] = key_right;
    v[code(XK_Up)]    = key_up;
    v[code(XK_Down)]  = key_down;

    v[code(XK_1)] = key_1;
    v[code(XK_2)] = key_2;
    v[code(XK_3)] = key_3;
    v[code(XK_4)] = key_4;
    v[code(XK_5)] = key_5;
    v[code(XK_6)] = key_6;
    v[code(XK_7)] = key_7;
    v[code(XK_8)] = key_8;
    v[code(XK_9)] = key_9;
    v[code(XK_0)] = key_0;

    v[code(XK_A)] = key_a;
    v[code(XK_B)] = key_b;
    v[code(XK_C)] = key_c;
    v[code(XK_D)] = key_d;
    v[code(XK_E)] = key_e;
    v[code(XK_F)] = key_f;
    v[code(XK_G)] = key_g;
    v[code(XK_H)] = key_h;
    v[code(XK_I)] = key_i;
    v[code(XK_J)] = key_j;
    v[code(XK_K)] = key_k;
    v[code(XK_L)] = key_l;
    v[code(XK_M)] = key_m;
    v[code(XK_N)] = key_n;
    v[code(XK_O)] = key_o;
    v[code(XK_P)] = key_p;
    v[code(XK_Q)] = key_q;
    v[code(XK_R)] = key_r;
    v[code(XK_S)] = key_s;
    v[code(XK_T)] = key_t;
    v[code(XK_V)] = key_v;
    v[code(XK_W)] = key_w;
    v[code(XK_X)] = key_x;
    v[code(XK_Y)] = key_y;
    v[code(XK_Z)] = key_z;

    v[code(XK_space)]      = key_space;
    v[code(XK_braceleft)]  = key_open;
    v[code(XK_braceright)] = key_close;
    v[code(XK_colon)]      = key_semicolon;
    v[code(XK_quotedbl)]   = key_quote;
    v[code(XK_asciitilde)] = key_tilda;
    v[code(XK_backslash)]  = key_backslash;
    v[code(XK_comma)]      = key_comma;
    v[code(XK_greater)]    = key_period;
    v[code(XK_question)]   = key_slash;

    v[code(XK_F1)]  = key_f1;
    v[code(XK_F2)]  = key_f2;
    v[code(XK_F3)]  = key_f3;
    v[code(XK_F4)]  = key_f4;
    v[code(XK_F5)]  = key_f5;
    v[code(XK_F6)]  = key_f6;
    v[code(XK_F7)]  = key_f7;
    v[code(XK_F8)]  = key_f8;
    v[code(XK_F9)]  = key_f9;
    v[code(XK_F10)] = key_f10;
    v[code(XK_F11)] = key_f11;
    v[code(XK_F12)] = key_f12;

    v[code(XK_Control_L)] = key_lcontrol;
    v[code(XK_Control_R)] = key_rcontrol;
    v[code(XK_Shift_L)]   = key_lshift;
    v[code(XK_Shift_R)]   = key_rshift;
    v[code(XK_Alt_L)]     = key_lalt;
    v[code(XK_Alt_R)]     = key_ralt;

    v[code(XK_Escape)]    = key_escape;
    v[code(XK_BackSpace)] = key_backspace;
    v[code(XK_Tab)]       = key_tab;
    v[code(XK_Return)]    = key_enter;
    v[code(XK_Print)]     = key_printscreen;
    v[code(XK_Delete)]    = key_delete;
    v[code(XK_Pause)]     = key_pause;
    v[code(XK_Insert)]    = key_insert;
    v[code(XK_Home)]      = key_home;
    v[code(XK_End)]       = key_end;
    v[code(XK_Page_Up)]   = key_pageup;
    v[code(XK_Page_Down)] = key_pagedown;

    v[code(XK_KP_0)] = key_numpad0;
    v[code(XK_KP_1)] = key_numpad1;
    v[code(XK_KP_2)] = key_numpad2;
    v[code(XK_KP_3)] = key_numpad3;
    v[code(XK_KP_4)] = key_numpad4;
    v[code(XK_KP_5)] = key_numpad5;
    v[code(XK_KP_6)] = key_numpad6;
    v[code(XK_KP_7)] = key_numpad7;
    v[code(XK_KP_8)] = key_numpad8;
    v[code(XK_KP_9)] = key_numpad9;

    v[code(XK_KP_Delete)]   = key_delete;
    v[code(XK_KP_Enter)]    = key_enter;
    v[code(XK_KP_Divide)]   = key_divide;
    v[code(XK_KP_Multiply)] = key_multiply;
    v[code(XK_KP_Add)]      = key_add;
    v[code(XK_KP_Subtract)] = key_subtract;
    v[code(XK_KP_Decimal)]  = key_separator;

    v[code(XK_Caps_Lock)]   = key_capslock_toggle;
    v[code(XK_Num_Lock)]    = key_numlock_toggle;
    v[code(XK_Scroll_Lock)] = key_scrolllock_toggle;

    v[0] = 0;
    return v;
  }

  void input::adjust_and_set_keymap(keymap_table v) noexcept {
    auto const find_empty = [&](uint8_t &n) -> uint8_t {
      for (n = 1; n != 0; n++)
        if (v[n] == 0)
          return n;
      return 0;
    };

    v[find_empty(in_lbutton)]    = key_lbutton;
    v[find_empty(in_rbutton)]    = key_rbutton;
    v[find_empty(in_mbutton)]    = key_mbutton;
    v[find_empty(in_xbutton1)]   = key_xbutton1;
    v[find_empty(in_xbutton2)]   = key_xbutton2;
    v[find_empty(in_shift)]      = key_shift;
    v[find_empty(in_control)]    = key_control;
    v[find_empty(in_alt)]        = key_alt;
    v[find_empty(in_capslock)]   = key_capslock;
    v[find_empty(in_numlock)]    = key_numlock;
    v[find_empty(in_scrolllock)] = key_scrolllock;
    v[0]                         = 0;

    set_keymap(v);
  }

  void input::toggle(sl::index key,
                     bool      is_down,
                     bool     &state) noexcept {
    if (is_down)
      state = !state;

    if (state == is_down)
      update_key(key, is_down);
  }
}
