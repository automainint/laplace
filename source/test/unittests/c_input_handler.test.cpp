/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/input_handler.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::input_event, core::is_key_down, core::is_key_up,
      core::get_wheel_delta, std::string_view;
  namespace keys = core::keys;

  TEST(core, input_handler_key_state) {
    auto key_down = input_event {
      .key = keys::key_space, .delta = input_event::delta_key_down
    };
    auto key_up = input_event { .key   = keys::key_space,
                                .delta = input_event::delta_key_up };
    EXPECT_TRUE(is_key_down(key_down));
    EXPECT_FALSE(is_key_up(key_down));
    EXPECT_FALSE(is_key_down(key_up));
    EXPECT_TRUE(is_key_up(key_up));
  }

  TEST(core, input_handler_wheel_state) {
    disable_log();

    auto wheel = input_event { .key = keys::key_wheel, .delta = 123 };
    auto key   = input_event { .key = keys::key_space, .delta = 123 };

    if (!_unsafe) {
      EXPECT_FALSE(is_key_down(wheel));
      EXPECT_FALSE(is_key_up(wheel));
      EXPECT_EQ(get_wheel_delta(key), 0);
    } else {
      EXPECT_TRUE(is_key_down(wheel));
      EXPECT_FALSE(is_key_down(wheel));
      EXPECT_EQ(get_wheel_delta(key), 123);
    }

    EXPECT_EQ(get_wheel_delta(wheel), 123);

    setup_log({});
  }
}
