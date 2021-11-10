/*  Copyright (c) 2021 Mitya Selivanov
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
  using ui::basic_widget, std::make_shared;

  TEST(ui, basic_widget_expired_at_construct) {
    auto w = basic_widget {};
    EXPECT_TRUE(w.is_expired());
    EXPECT_FALSE(w.has_attached_expired());
  }

  TEST(ui, basic_widget_expired_childs) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    foo->attach(bar);
    EXPECT_TRUE(foo->has_attached_expired());
    EXPECT_TRUE(bar->get_parent());
  }

  TEST(ui, basic_widget_set_expired) {
    auto w = basic_widget {};
    w.set_expired(false);
    EXPECT_FALSE(w.is_expired());
  }

  TEST(ui, basic_widget_attach_null) {
    auto w = basic_widget {};
    EXPECT_LT(w.attach({}), 0);
    w.detach({});
  }

  TEST(ui, basic_widget_visible_at_construct) {
    auto w = basic_widget {};
    EXPECT_TRUE(w.is_visible());
  }

  TEST(ui, basic_widget_enabled_at_construct) {
    auto w = basic_widget {};
    EXPECT_TRUE(w.is_visible());
  }

  TEST(ui, basic_widget_not_attached_at_construct) {
    auto w = basic_widget {};
    EXPECT_FALSE(w.is_attached());
  }

  TEST(ui, basic_widget_tick) {
    auto w = basic_widget {};
    w.tick(0, {});
  }

  TEST(ui, basic_widget_tick_childs) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    foo->attach(bar);
    foo->tick(0, {});
  }

  TEST(ui, basic_widget_render) {
    auto w = basic_widget {};
    w.render({ .prepare = []() {} });
    EXPECT_FALSE(w.is_expired());
  }

  TEST(ui, basic_widget_rect) {
    auto w = basic_widget {};
    w.set_rect({ .x = 10, .y = 20, .width = 60, .height = 30 });
    EXPECT_EQ(w.get_rect().x, 10);
    EXPECT_EQ(w.get_rect().y, 20);
    EXPECT_EQ(w.get_rect().width, 60);
    EXPECT_EQ(w.get_rect().height, 30);
  }

  TEST(ui, basic_widget_render_childs) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();
    auto sub = make_shared<basic_widget>();

    foo->set_rect({ .x = 0, .y = 0, .width = 100, .height = 100 });
    bar->set_rect({ .x = 0, .y = 0, .width = 50, .height = 100 });
    bus->set_rect({ .x = 50, .y = 0, .width = 50, .height = 100 });
    sub->set_rect({ .x = 0, .y = 0, .width = 50, .height = 100 });

    foo->attach(bar);
    foo->attach(bus);
    bar->attach(sub);
    foo->render({ .prepare = []() {} });

    EXPECT_FALSE(foo->is_expired());
    EXPECT_FALSE(bar->is_expired());
    EXPECT_FALSE(bus->is_expired());
    EXPECT_FALSE(sub->is_expired());
  }

  TEST(ui, basic_widget_render_skip) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();

    foo->set_rect({ .x = 0, .y = 0, .width = 100, .height = 100 });
    bar->set_rect({ .x = 200, .y = 0, .width = 50, .height = 50 });
    foo->attach(bar);
    foo->render({ .prepare = []() {} });

    EXPECT_FALSE(foo->is_expired());
    EXPECT_TRUE(bar->is_expired());
  }

  TEST(ui, basic_widget_absolute_rect) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();

    foo->set_rect(
        { .x = 100, .y = 100, .width = 100, .height = 100 });
    bar->set_rect({ .x = 200, .y = 0, .width = 50, .height = 60 });

    foo->attach(bar);
    foo->render({ .prepare = []() {} });

    EXPECT_EQ(bar->get_absolute_x(), 300);
    EXPECT_EQ(bar->get_absolute_y(), 100);
    EXPECT_EQ(bar->get_absolute_rect().x, 300);
    EXPECT_EQ(bar->get_absolute_rect().y, 100);
    EXPECT_EQ(bar->get_absolute_rect().width, 50);
    EXPECT_EQ(bar->get_absolute_rect().height, 60);
  }

  TEST(ui, basic_widget_absolute_rect_hidden) {
    auto foo    = make_shared<basic_widget>();
    auto bar    = make_shared<basic_widget>();
    auto hidden = make_shared<basic_widget>();

    foo->set_rect({ .x = 0, .y = 0, .width = 100, .height = 100 });
    bar->set_rect({ .x = 200, .y = 20, .width = 50, .height = 50 });
    hidden->set_rect({ .x = 0, .y = 0, .width = 20, .height = 20 });

    foo->attach(bar);
    bar->attach(hidden);
    foo->render({ .prepare = []() {} });

    EXPECT_EQ(hidden->get_absolute_x(), 200);
    EXPECT_EQ(hidden->get_absolute_y(), 20);
  }

  TEST(ui, basic_widget_detach_one) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();

    foo->attach(bar);
    foo->detach(bar);

    EXPECT_EQ(foo->get_attached_widget_count(), 0);
  }

  TEST(ui, basic_widget_detach_first) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);
    foo->detach(bar);

    EXPECT_EQ(foo->get_attached_widget_count(), 1);
    EXPECT_EQ(foo->get_attached_widget(0), bus);
    EXPECT_FALSE(bar->get_parent());
  }

  TEST(ui, basic_widget_detach_last) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);
    foo->detach(bus);

    EXPECT_EQ(foo->get_attached_widget_count(), 1);
    EXPECT_EQ(foo->get_attached_widget(0), bar);
    EXPECT_FALSE(bus->get_parent());
  }

  TEST(ui, basic_widget_detach_first_by_index) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);
    foo->detach(0);

    EXPECT_EQ(foo->get_attached_widget_count(), 1);
    EXPECT_EQ(foo->get_attached_widget(0), bus);
    EXPECT_FALSE(bar->get_parent());
  }

  TEST(ui, basic_widget_detach_last_by_index) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);
    foo->detach(1);

    EXPECT_EQ(foo->get_attached_widget_count(), 1);
    EXPECT_EQ(foo->get_attached_widget(0), bar);
    EXPECT_FALSE(bus->get_parent());
  }

  TEST(ui, basic_widget_clear) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);
    foo->clear();

    EXPECT_EQ(foo->get_attached_widget_count(), 0);
    EXPECT_FALSE(bar->get_parent());
    EXPECT_FALSE(bus->get_parent());
  }

  TEST(ui, basic_widget_set_focus) {
    auto w = basic_widget {};
    EXPECT_FALSE(w.has_focus());
    w.set_focus(true);
    EXPECT_TRUE(w.has_focus());
    w.set_focus(false);
    EXPECT_FALSE(w.has_focus());
  }

  TEST(ui, basic_widget_switch_focus) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();
    auto bus = make_shared<basic_widget>();

    foo->attach(bar);
    foo->attach(bus);

    bar->set_focus(true);
    EXPECT_TRUE(bar->has_focus());
    EXPECT_FALSE(bus->has_focus());
    bus->set_focus(true);
    EXPECT_FALSE(bar->has_focus());
    EXPECT_TRUE(bus->has_focus());
  }

  class _focus_handler : public basic_widget {
  public:
    _focus_handler() noexcept {
      set_handler(true);
    }

    ~_focus_handler() override = default;
  };

  TEST(ui, basic_widget_handle_focus) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<_focus_handler>();
    auto bus = make_shared<_focus_handler>();

    foo->attach(bar);
    foo->attach(bus);
    bar->set_focus(true);
    foo->next_tab();

    EXPECT_FALSE(bar->has_focus());
    EXPECT_TRUE(bus->has_focus());
  }

  TEST(ui, basic_widget_first_tab) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<_focus_handler>();
    auto bus = make_shared<_focus_handler>();

    foo->attach(bar);
    foo->attach(bus);
    foo->next_tab();

    EXPECT_TRUE(bar->has_focus());
    EXPECT_FALSE(bus->has_focus());
  }

  TEST(ui, basic_widget_event_allowed) {
    auto foo = make_shared<basic_widget>();
    auto bar = make_shared<basic_widget>();

    foo->set_rect({ .x = 50, .y = 20, .width = 200, .height = 100 });
    bar->set_rect({ .x = 10, .y = 10, .width = 30, .height = 60 });
    foo->attach(bar);
    foo->render({ .prepare = []() {} });

    EXPECT_FALSE(bar->event_allowed(59, 40));
    EXPECT_FALSE(bar->event_allowed(70, 29));
    EXPECT_TRUE(bar->event_allowed(60, 30));
    EXPECT_TRUE(bar->event_allowed(89, 89));
    EXPECT_FALSE(bar->event_allowed(90, 40));
    EXPECT_FALSE(bar->event_allowed(70, 90));
  }

  TEST(ui, basic_widget_level) {
    auto w = basic_widget {};
    w.set_level(1);
    EXPECT_EQ(w.get_level(), 1);
    w.set_level(3);
    EXPECT_EQ(w.get_level(), 3);
  }

  TEST(ui, basic_widget_set_visible) {
    auto w = basic_widget {};
    w.set_visible(false);
    EXPECT_FALSE(w.is_visible());
    w.set_visible(true);
    EXPECT_TRUE(w.is_visible());
  }

  TEST(ui, basic_widget_set_enabled) {
    auto w = basic_widget {};
    w.set_enabled(false);
    EXPECT_FALSE(w.is_enabled());
    w.set_enabled(true);
    EXPECT_TRUE(w.is_enabled());
  }
}
