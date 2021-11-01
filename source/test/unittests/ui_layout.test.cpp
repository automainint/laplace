/*  test/unittests/ui_layout.test.h
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

#include "../../laplace/ui/basic_widget.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using ui::basic_widget, ui::column_layout, std::make_shared,
      std::tuple;

  TEST(ui, column_layout) {
    auto root = make_shared<basic_widget>();
    auto foo  = make_shared<basic_widget>();
    auto bar  = make_shared<basic_widget>();

    root->set_rect({ .x = 0, .y = 0, .width = 100, .height = 200 });
    root->attach(foo);
    root->attach(bar);
    root->set_layout(column_layout(80, 40, 5));

    EXPECT_EQ(foo->get_rect().x, 15);
    EXPECT_EQ(foo->get_rect().y, 5);
    EXPECT_EQ(foo->get_rect().width, 70);
    EXPECT_EQ(foo->get_rect().height, 30);

    EXPECT_EQ(bar->get_rect().x, 15);
    EXPECT_EQ(bar->get_rect().y, 45);
    EXPECT_EQ(bar->get_rect().width, 70);
    EXPECT_EQ(bar->get_rect().height, 30);
  }

  TEST(ui, column_layout2) {
    auto root = make_shared<basic_widget>();
    auto foo  = make_shared<basic_widget>();
    auto bar  = make_shared<basic_widget>();
    auto bus  = make_shared<basic_widget>();

    root->set_rect({ .x = 0, .y = 0, .width = 100, .height = 200 });
    root->attach(foo);
    root->attach(bar);
    root->attach(bus);
    root->set_layout(column_layout(tuple { 50, 30 }, 40, 5));

    EXPECT_EQ(foo->get_rect().x, 15);
    EXPECT_EQ(foo->get_rect().y, 5);
    EXPECT_EQ(foo->get_rect().width, 40);
    EXPECT_EQ(foo->get_rect().height, 30);

    EXPECT_EQ(bar->get_rect().x, 65);
    EXPECT_EQ(bar->get_rect().y, 5);
    EXPECT_EQ(bar->get_rect().width, 20);
    EXPECT_EQ(bar->get_rect().height, 30);

    EXPECT_EQ(bus->get_rect().x, 15);
    EXPECT_EQ(bus->get_rect().y, 45);
    EXPECT_EQ(bus->get_rect().width, 40);
    EXPECT_EQ(bus->get_rect().height, 30);
  }
}
