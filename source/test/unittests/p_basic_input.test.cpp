/*  test/unittests/p_basic_input.test.cpp
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

#include "../../laplace/platform/basic_input.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using platform::basic_input;
  using namespace core::keys;

  TEST(platform, basic_input_key_state) {
    auto in = basic_input {};

    for (sl::index key = 0; key < in.key_count; key++) {
      EXPECT_FALSE(in.is_key_down(key));
      EXPECT_TRUE(in.is_key_up(key));
    }

    EXPECT_FALSE(in.is_capslock());
    EXPECT_FALSE(in.is_numlock());
    EXPECT_FALSE(in.is_scrolllock());
    EXPECT_FALSE(in.is_alt());
    EXPECT_FALSE(in.is_shift());
    EXPECT_FALSE(in.is_control());
  }

  TEST(platform, basic_input_invalid_key) {
    auto in = basic_input {};

    EXPECT_FALSE(in.is_key_down(-1));
    EXPECT_FALSE(in.is_key_down(5000));
  }

  TEST(platform, basic_input_mouse) {
    auto in = basic_input {};

    in.update_cursor(100, 200);

    EXPECT_EQ(in.get_cursor_x(), 100);
    EXPECT_EQ(in.get_cursor_y(), 200);

    in.refresh();
    in.update_cursor(120, 210);
    in.update_wheel(10);

    EXPECT_EQ(in.get_cursor_delta_x(), 20);
    EXPECT_EQ(in.get_cursor_delta_y(), 10);
    EXPECT_EQ(in.get_wheel_delta(), 10);
  }

  TEST(platform, basic_input_wheel_scale) {
    auto in = basic_input {};

    in.set_wheel_scale(10);
    in.update_wheel(-2);
    EXPECT_EQ(in.get_wheel_delta(), -20);
    in.update_wheel(5);
    EXPECT_EQ(in.get_wheel_delta(), 30);
    in.refresh();
    EXPECT_EQ(in.get_wheel_delta(), 0);
  }

  TEST(platform, basic_input_get_events) {
    auto in = basic_input {};
    EXPECT_TRUE(in.get_events().empty());
  }

  TEST(platform, basic_input_keymap) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    EXPECT_EQ(in.map_key(5), 5);
    EXPECT_EQ(in.map_key(-1), 0);
    EXPECT_EQ(in.map_key(400), 0);

    in.update_key(10, true);
    EXPECT_TRUE(in.is_key_down(10));

    in.reset_keyboard();
    EXPECT_FALSE(in.is_key_down(10));

    in.update_key(-1, true);
    in.update_key(400, true);
    EXPECT_FALSE(in.is_key_down(-1));
    EXPECT_FALSE(in.is_key_down(400));
  }

  TEST(platform, basic_input_lowercase) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    EXPECT_EQ(in.to_char(key_1), U'1');
    EXPECT_EQ(in.to_char(key_2), U'2');
    EXPECT_EQ(in.to_char(key_3), U'3');
    EXPECT_EQ(in.to_char(key_4), U'4');
    EXPECT_EQ(in.to_char(key_5), U'5');
    EXPECT_EQ(in.to_char(key_6), U'6');
    EXPECT_EQ(in.to_char(key_7), U'7');
    EXPECT_EQ(in.to_char(key_8), U'8');
    EXPECT_EQ(in.to_char(key_9), U'9');
    EXPECT_EQ(in.to_char(key_0), U'0');

    EXPECT_EQ(in.to_char(key_a), U'a');
    EXPECT_EQ(in.to_char(key_b), U'b');
    EXPECT_EQ(in.to_char(key_c), U'c');
    EXPECT_EQ(in.to_char(key_d), U'd');
    EXPECT_EQ(in.to_char(key_e), U'e');
    EXPECT_EQ(in.to_char(key_f), U'f');
    EXPECT_EQ(in.to_char(key_g), U'g');
    EXPECT_EQ(in.to_char(key_h), U'h');
    EXPECT_EQ(in.to_char(key_i), U'i');
    EXPECT_EQ(in.to_char(key_j), U'j');
    EXPECT_EQ(in.to_char(key_k), U'k');
    EXPECT_EQ(in.to_char(key_l), U'l');
    EXPECT_EQ(in.to_char(key_m), U'm');
    EXPECT_EQ(in.to_char(key_n), U'n');
    EXPECT_EQ(in.to_char(key_o), U'o');
    EXPECT_EQ(in.to_char(key_p), U'p');
    EXPECT_EQ(in.to_char(key_q), U'q');
    EXPECT_EQ(in.to_char(key_r), U'r');
    EXPECT_EQ(in.to_char(key_s), U's');
    EXPECT_EQ(in.to_char(key_t), U't');
    EXPECT_EQ(in.to_char(key_u), U'u');
    EXPECT_EQ(in.to_char(key_v), U'v');
    EXPECT_EQ(in.to_char(key_w), U'w');
    EXPECT_EQ(in.to_char(key_x), U'x');
    EXPECT_EQ(in.to_char(key_y), U'y');
    EXPECT_EQ(in.to_char(key_z), U'z');

    EXPECT_EQ(in.to_char(key_open), U'[');
    EXPECT_EQ(in.to_char(key_close), U']');
    EXPECT_EQ(in.to_char(key_semicolon), U';');
    EXPECT_EQ(in.to_char(key_quote), U'\'');
    EXPECT_EQ(in.to_char(key_comma), U',');
    EXPECT_EQ(in.to_char(key_period), U'.');
    EXPECT_EQ(in.to_char(key_slash), U'/');
    EXPECT_EQ(in.to_char(key_backslash), U'\\');
    EXPECT_EQ(in.to_char(key_tilda), U'`');

    EXPECT_EQ(in.to_char(key_minus), U'-');
    EXPECT_EQ(in.to_char(key_equals), U'=');
    EXPECT_EQ(in.to_char(key_space), U' ');
  }

  TEST(platform, basic_input_shift) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.update_key(key_shift, true);

    EXPECT_EQ(in.to_char(key_1), U'!');
    EXPECT_EQ(in.to_char(key_2), U'@');
    EXPECT_EQ(in.to_char(key_3), U'#');
    EXPECT_EQ(in.to_char(key_4), U'$');
    EXPECT_EQ(in.to_char(key_5), U'%');
    EXPECT_EQ(in.to_char(key_6), U'^');
    EXPECT_EQ(in.to_char(key_7), U'&');
    EXPECT_EQ(in.to_char(key_8), U'*');
    EXPECT_EQ(in.to_char(key_9), U'(');
    EXPECT_EQ(in.to_char(key_0), U')');

    EXPECT_EQ(in.to_char(key_a), U'A');
    EXPECT_EQ(in.to_char(key_b), U'B');
    EXPECT_EQ(in.to_char(key_c), U'C');
    EXPECT_EQ(in.to_char(key_d), U'D');
    EXPECT_EQ(in.to_char(key_e), U'E');
    EXPECT_EQ(in.to_char(key_f), U'F');
    EXPECT_EQ(in.to_char(key_g), U'G');
    EXPECT_EQ(in.to_char(key_h), U'H');
    EXPECT_EQ(in.to_char(key_i), U'I');
    EXPECT_EQ(in.to_char(key_j), U'J');
    EXPECT_EQ(in.to_char(key_k), U'K');
    EXPECT_EQ(in.to_char(key_l), U'L');
    EXPECT_EQ(in.to_char(key_m), U'M');
    EXPECT_EQ(in.to_char(key_n), U'N');
    EXPECT_EQ(in.to_char(key_o), U'O');
    EXPECT_EQ(in.to_char(key_p), U'P');
    EXPECT_EQ(in.to_char(key_q), U'Q');
    EXPECT_EQ(in.to_char(key_r), U'R');
    EXPECT_EQ(in.to_char(key_s), U'S');
    EXPECT_EQ(in.to_char(key_t), U'T');
    EXPECT_EQ(in.to_char(key_u), U'U');
    EXPECT_EQ(in.to_char(key_v), U'V');
    EXPECT_EQ(in.to_char(key_w), U'W');
    EXPECT_EQ(in.to_char(key_x), U'X');
    EXPECT_EQ(in.to_char(key_y), U'Y');
    EXPECT_EQ(in.to_char(key_z), U'Z');

    EXPECT_EQ(in.to_char(key_open), U'{');
    EXPECT_EQ(in.to_char(key_close), U'}');
    EXPECT_EQ(in.to_char(key_semicolon), U':');
    EXPECT_EQ(in.to_char(key_quote), U'"');
    EXPECT_EQ(in.to_char(key_comma), U'<');
    EXPECT_EQ(in.to_char(key_period), U'>');
    EXPECT_EQ(in.to_char(key_slash), U'?');
    EXPECT_EQ(in.to_char(key_backslash), U'|');
    EXPECT_EQ(in.to_char(key_tilda), U'~');

    EXPECT_EQ(in.to_char(key_minus), U'_');
    EXPECT_EQ(in.to_char(key_equals), U'+');
    EXPECT_EQ(in.to_char(key_space), U' ');
  }

  TEST(platform, basic_input_capslock) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.update_key(key_capslock, true);

    EXPECT_EQ(in.to_char(key_1), U'1');
    EXPECT_EQ(in.to_char(key_2), U'2');
    EXPECT_EQ(in.to_char(key_3), U'3');
    EXPECT_EQ(in.to_char(key_4), U'4');
    EXPECT_EQ(in.to_char(key_5), U'5');
    EXPECT_EQ(in.to_char(key_6), U'6');
    EXPECT_EQ(in.to_char(key_7), U'7');
    EXPECT_EQ(in.to_char(key_8), U'8');
    EXPECT_EQ(in.to_char(key_9), U'9');
    EXPECT_EQ(in.to_char(key_0), U'0');

    EXPECT_EQ(in.to_char(key_a), U'A');
    EXPECT_EQ(in.to_char(key_b), U'B');
    EXPECT_EQ(in.to_char(key_c), U'C');
    EXPECT_EQ(in.to_char(key_d), U'D');
    EXPECT_EQ(in.to_char(key_e), U'E');
    EXPECT_EQ(in.to_char(key_f), U'F');
    EXPECT_EQ(in.to_char(key_g), U'G');
    EXPECT_EQ(in.to_char(key_h), U'H');
    EXPECT_EQ(in.to_char(key_i), U'I');
    EXPECT_EQ(in.to_char(key_j), U'J');
    EXPECT_EQ(in.to_char(key_k), U'K');
    EXPECT_EQ(in.to_char(key_l), U'L');
    EXPECT_EQ(in.to_char(key_m), U'M');
    EXPECT_EQ(in.to_char(key_n), U'N');
    EXPECT_EQ(in.to_char(key_o), U'O');
    EXPECT_EQ(in.to_char(key_p), U'P');
    EXPECT_EQ(in.to_char(key_q), U'Q');
    EXPECT_EQ(in.to_char(key_r), U'R');
    EXPECT_EQ(in.to_char(key_s), U'S');
    EXPECT_EQ(in.to_char(key_t), U'T');
    EXPECT_EQ(in.to_char(key_u), U'U');
    EXPECT_EQ(in.to_char(key_v), U'V');
    EXPECT_EQ(in.to_char(key_w), U'W');
    EXPECT_EQ(in.to_char(key_x), U'X');
    EXPECT_EQ(in.to_char(key_y), U'Y');
    EXPECT_EQ(in.to_char(key_z), U'Z');

    EXPECT_EQ(in.to_char(key_open), U'[');
    EXPECT_EQ(in.to_char(key_close), U']');
    EXPECT_EQ(in.to_char(key_semicolon), U';');
    EXPECT_EQ(in.to_char(key_quote), U'\'');
    EXPECT_EQ(in.to_char(key_comma), U',');
    EXPECT_EQ(in.to_char(key_period), U'.');
    EXPECT_EQ(in.to_char(key_slash), U'/');
    EXPECT_EQ(in.to_char(key_backslash), U'\\');
    EXPECT_EQ(in.to_char(key_tilda), U'`');

    EXPECT_EQ(in.to_char(key_minus), U'-');
    EXPECT_EQ(in.to_char(key_equals), U'=');
    EXPECT_EQ(in.to_char(key_space), U' ');
  }

  TEST(platform, basic_input_capslock_and_shift) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.update_key(key_capslock, true);
    in.update_key(key_shift, true);

    EXPECT_EQ(in.to_char(key_1), U'!');
    EXPECT_EQ(in.to_char(key_2), U'@');
    EXPECT_EQ(in.to_char(key_3), U'#');
    EXPECT_EQ(in.to_char(key_4), U'$');
    EXPECT_EQ(in.to_char(key_5), U'%');
    EXPECT_EQ(in.to_char(key_6), U'^');
    EXPECT_EQ(in.to_char(key_7), U'&');
    EXPECT_EQ(in.to_char(key_8), U'*');
    EXPECT_EQ(in.to_char(key_9), U'(');
    EXPECT_EQ(in.to_char(key_0), U')');

    EXPECT_EQ(in.to_char(key_a), U'a');
    EXPECT_EQ(in.to_char(key_b), U'b');
    EXPECT_EQ(in.to_char(key_c), U'c');
    EXPECT_EQ(in.to_char(key_d), U'd');
    EXPECT_EQ(in.to_char(key_e), U'e');
    EXPECT_EQ(in.to_char(key_f), U'f');
    EXPECT_EQ(in.to_char(key_g), U'g');
    EXPECT_EQ(in.to_char(key_h), U'h');
    EXPECT_EQ(in.to_char(key_i), U'i');
    EXPECT_EQ(in.to_char(key_j), U'j');
    EXPECT_EQ(in.to_char(key_k), U'k');
    EXPECT_EQ(in.to_char(key_l), U'l');
    EXPECT_EQ(in.to_char(key_m), U'm');
    EXPECT_EQ(in.to_char(key_n), U'n');
    EXPECT_EQ(in.to_char(key_o), U'o');
    EXPECT_EQ(in.to_char(key_p), U'p');
    EXPECT_EQ(in.to_char(key_q), U'q');
    EXPECT_EQ(in.to_char(key_r), U'r');
    EXPECT_EQ(in.to_char(key_s), U's');
    EXPECT_EQ(in.to_char(key_t), U't');
    EXPECT_EQ(in.to_char(key_u), U'u');
    EXPECT_EQ(in.to_char(key_v), U'v');
    EXPECT_EQ(in.to_char(key_w), U'w');
    EXPECT_EQ(in.to_char(key_x), U'x');
    EXPECT_EQ(in.to_char(key_y), U'y');
    EXPECT_EQ(in.to_char(key_z), U'z');
    
    EXPECT_EQ(in.to_char(key_open), U'{');
    EXPECT_EQ(in.to_char(key_close), U'}');
    EXPECT_EQ(in.to_char(key_semicolon), U':');
    EXPECT_EQ(in.to_char(key_quote), U'"');
    EXPECT_EQ(in.to_char(key_comma), U'<');
    EXPECT_EQ(in.to_char(key_period), U'>');
    EXPECT_EQ(in.to_char(key_slash), U'?');
    EXPECT_EQ(in.to_char(key_backslash), U'|');
    EXPECT_EQ(in.to_char(key_tilda), U'~');

    EXPECT_EQ(in.to_char(key_minus), U'_');
    EXPECT_EQ(in.to_char(key_equals), U'+');
    EXPECT_EQ(in.to_char(key_space), U' ');
  }

  TEST(platform, basic_input_numlock) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.update_key(key_numlock, true);

    EXPECT_EQ(in.to_char(key_numpad0), U'0');
    EXPECT_EQ(in.to_char(key_numpad1), U'1');
    EXPECT_EQ(in.to_char(key_numpad2), U'2');
    EXPECT_EQ(in.to_char(key_numpad3), U'3');
    EXPECT_EQ(in.to_char(key_numpad4), U'4');
    EXPECT_EQ(in.to_char(key_numpad5), U'5');
    EXPECT_EQ(in.to_char(key_numpad6), U'6');
    EXPECT_EQ(in.to_char(key_numpad7), U'7');
    EXPECT_EQ(in.to_char(key_numpad8), U'8');
    EXPECT_EQ(in.to_char(key_numpad9), U'9');

    EXPECT_EQ(in.to_char(key_add), U'+');
    EXPECT_EQ(in.to_char(key_subtract), U'-');
    EXPECT_EQ(in.to_char(key_multiply), U'*');
    EXPECT_EQ(in.to_char(key_divide), U'/');
    EXPECT_EQ(in.to_char(key_decimal), U'.');
  }

  TEST(platform, basic_input_control_keys) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    EXPECT_EQ(in.to_char(key_tab), ctrl_tab);
    EXPECT_EQ(in.to_char(key_backspace), ctrl_backspace);
    EXPECT_EQ(in.to_char(key_enter), ctrl_return);
    EXPECT_EQ(in.to_char(key_delete), ctrl_delete);
    EXPECT_EQ(in.to_char(key_left), ctrl_left);
    EXPECT_EQ(in.to_char(key_up), ctrl_up);
    EXPECT_EQ(in.to_char(key_right), ctrl_right);
    EXPECT_EQ(in.to_char(key_down), ctrl_down);

    EXPECT_EQ(in.to_char(key_numpad4), ctrl_left);
    EXPECT_EQ(in.to_char(key_numpad8), ctrl_up);
    EXPECT_EQ(in.to_char(key_numpad6), ctrl_right);
    EXPECT_EQ(in.to_char(key_numpad2), ctrl_down);

    in.update_key(key_numlock, true);
    in.update_key(key_shift, true);

    EXPECT_EQ(in.to_char(key_numpad4), ctrl_left);
    EXPECT_EQ(in.to_char(key_numpad8), ctrl_up);
    EXPECT_EQ(in.to_char(key_numpad6), ctrl_right);
    EXPECT_EQ(in.to_char(key_numpad2), ctrl_down);
  }

  TEST(platform, basic_input_events) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.update_cursor(10, 20);
    in.update_key(key_space, true);
    in.update_cursor(20, 30);
    in.update_key(key_space, false);
    in.update_wheel(80);

    EXPECT_EQ(in.get_events().size(), 3);

    if (in.get_events().size() >= 1) {
      EXPECT_TRUE(core::is_key_down(in.get_events()[0]));
      EXPECT_EQ(in.get_events()[0].key, key_space);
      EXPECT_EQ(in.get_events()[0].character, U' ');
      EXPECT_EQ(in.get_events()[0].cursor_x, 10);
      EXPECT_EQ(in.get_events()[0].cursor_y, 20);
    }

    if (in.get_events().size() >= 2) {
      EXPECT_TRUE(core::is_key_up(in.get_events()[1]));
      EXPECT_EQ(in.get_events()[1].key, key_space);
      EXPECT_EQ(in.get_events()[1].cursor_x, 20);
      EXPECT_EQ(in.get_events()[1].cursor_y, 30);
    }

    if (in.get_events().size() >= 3) {
      EXPECT_EQ(in.get_events()[2].key, key_wheel);
      EXPECT_EQ(in.get_events()[2].delta, 80);
      EXPECT_EQ(in.get_events()[2].cursor_x, 20);
      EXPECT_EQ(in.get_events()[2].cursor_y, 30);
    }
  }

  TEST(platform, basic_input_char_period) {
    auto in = basic_input {};

    in.set_keymap([]() {
      auto v = basic_input::keymap_table {};
      for (sl::index i = 0; i < basic_input::key_count; i++) v[i] = i;
      return v;
    }());

    in.set_char_predelay(100);
    in.set_char_period(10);

    in.update_key(key_space, true);

    for (sl::index i = 0; i < 100; i++) in.tick(2);

    EXPECT_EQ(in.get_events().size(), 11);

    for (auto const &e : in.get_events()) {
      EXPECT_EQ(e.character, U' ');
    }

    in.update_key(key_space, false);
    for (sl::index i = 0; i < 100; i++) in.tick(2);

    EXPECT_EQ(in.get_events().size(), 12);
  }
}
