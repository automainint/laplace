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

    in.update_key(10, true);
    EXPECT_TRUE(in.is_key_down(10));

    in.reset_keyboard();
    EXPECT_FALSE(in.is_key_down(10));
  }
}
