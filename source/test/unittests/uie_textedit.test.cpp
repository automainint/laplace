/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/ui/elem/textedit.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using ui::elem::textedit, core::keys::ctrl_backspace,
      std::make_shared;

  TEST(ui, elem_textedit_default) {
    auto wid = make_shared<textedit>();
    EXPECT_TRUE(wid->get_text().empty());
    EXPECT_FALSE(wid->has_focus());
    EXPECT_EQ(wid->get_cursor(), 0);
    EXPECT_EQ(wid->get_selection(), 0);
  }

  TEST(ui, elem_testedit_process_event) {
    auto wid = make_shared<textedit>();

    wid->set_focus(false);
    wid->process_event({ .character = U'X' });
    EXPECT_EQ(wid->get_cursor(), 0);
    EXPECT_EQ(wid->get_text(), u8"");

    wid->set_focus(true);
    wid->process_event({ .character = U'F' });
    EXPECT_EQ(wid->get_cursor(), 1);
    wid->process_event({ .character = U'o' });
    EXPECT_EQ(wid->get_cursor(), 2);
    wid->process_event({ .character = U'o' });
    EXPECT_EQ(wid->get_cursor(), 3);
    EXPECT_EQ(wid->get_text(), u8"Foo");

    wid->process_event({ .character = ctrl_backspace });
    EXPECT_EQ(wid->get_cursor(), 2);
    wid->process_event({ .character = ctrl_backspace });
    EXPECT_EQ(wid->get_cursor(), 1);
    EXPECT_EQ(wid->get_text(), u8"F");
  }
}
