/*  laplace/platform/keys.h
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

#ifndef laplace_platform_keys_h
#define laplace_platform_keys_h

#include <cstdint>

namespace laplace::platform {
  /*  Input control codes based on ASCII.
   */
  enum input_control : char32_t {
    ctrl_backspace = 0x08,
    ctrl_tab,
    ctrl_return,

    ctrl_left = 0x11,
    ctrl_up,
    ctrl_right,
    ctrl_down,

    ctrl_delete = 0x7f
  };

  /*  Input key codes.
   */
  enum key : int {
    key_lbutton = 0x01,
    key_rbutton,
    key_mbutton = 0x04,
    key_xbutton1,
    key_xbutton2,

    key_backspace = 0x08,
    key_tab       = 0x09,
    key_enter     = 0x0d,
    key_shift     = 0x10,
    key_control   = 0x11,
    key_alt       = 0x12,
    key_pause     = 0x13,
    key_capslock  = 0x14,
    key_space     = 0x20,
    key_pageup    = 0x21,
    key_pagedown  = 0x22,
    key_end       = 0x23,
    key_home      = 0x24,

    key_left = 0x25,
    key_up,
    key_right,
    key_down,

    key_printscreen = 0x2c,
    key_insert      = 0x2d,
    key_delete      = 0x2e,

    key_0 = '0',
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,

    key_a = 'A',
    key_b,
    key_c,
    key_d,
    key_e,
    key_f,
    key_g,
    key_h,
    key_i,
    key_j,
    key_k,
    key_l,
    key_m,
    key_n,
    key_o,
    key_p,
    key_q,
    key_r,
    key_s,
    key_t,
    key_u,
    key_v,
    key_w,
    key_x,
    key_y,
    key_z,

    key_numpad0 = 0x60,
    key_numpad1,
    key_numpad2,
    key_numpad3,
    key_numpad4,
    key_numpad5,
    key_numpad6,
    key_numpad7,
    key_numpad8,
    key_numpad9,

    key_multiply = 0x6a,
    key_add,
    key_separator,
    key_subtract,
    key_decimal,
    key_devide,

    key_f1 = 0x70,
    key_f2,
    key_f3,
    key_f4,
    key_f5,
    key_f6,
    key_f7,
    key_f8,
    key_f9,
    key_f10,
    key_f11,
    key_f12,
    key_f13,
    key_f14,
    key_f15,
    key_f16,
    key_f17,
    key_f18,
    key_f19,
    key_f20,
    key_f21,
    key_f22,
    key_f23,
    key_f24,

    key_numlock = 0x90,
    key_scrolllock,

    key_semicolon = 0xba, /* ;: */
    key_equals,           /* =+ */
    key_comma,            /* ,< */
    key_minus,            /* -_ */
    key_period,           /* .> */
    key_slash,            /* /? */
    key_tilda,            /* `~ */

    key_open = 0xdb, /* [{ */
    key_backslash,   /* \| */
    key_close,       /* ]} */
    key_quote,       /* '" */
  };
}

#endif
