/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/basic_entity.h"
#include "../../laplace/engine/world.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  using std::make_shared, std::thread, engine::basic_entity,
      engine::id_undefined, engine::world, engine::intval;

  namespace sets = engine::object::sets;

  class my_entity : public basic_entity {
  public:
    my_entity(my_entity const &) = default;
    my_entity(my_entity &&)      = default;
    auto operator=(my_entity const &) -> my_entity & = default;
    auto operator=(my_entity &&) -> my_entity & = default;

    my_entity() noexcept {
      setup_sets({ { sets::debug_value, 0, 0 } });
    }

    ~my_entity() noexcept override = default;

    void do_init() noexcept {
      init(index_of(sets::debug_value), 123);
      bytes_resize(1);
      bytes_init(0, 23);
      vec_resize(1);
      vec_init(0, 345);

      init(-1, 0);
      init(100, 0);
      bytes_init(-1, 0);
      bytes_init(100, 0);
      vec_init(-1, 0);
      vec_init(100, 0);
    }

    void do_desync() noexcept {
      desync();
    }

    void do_self_destruct(world &w) noexcept {
      self_destruct({ w, engine::access::sync });
    }

    void lock() noexcept {
      _mutex().lock();
    }

    void unlock() noexcept {
      _mutex().unlock();
    }
  };

  TEST(engine, basic_entity_default) {
    auto e = basic_entity {};

    EXPECT_EQ(e.get_count(), 2);
    EXPECT_NE(e.index_of(sets::is_dynamic), id_undefined);
    EXPECT_NE(e.index_of(sets::tick_period), id_undefined);
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 0);
  }

  TEST(engine, basic_entity_proto) {
    auto e = basic_entity { basic_entity::proto };

    EXPECT_EQ(e.get_count(), 2);
    EXPECT_NE(e.index_of(sets::is_dynamic), id_undefined);
    EXPECT_NE(e.index_of(sets::tick_period), id_undefined);
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 0);
  }

  TEST(engine, basic_entity_dummy) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::dummy };

    EXPECT_EQ(e.get_count(), 0);
    EXPECT_EQ(e.index_of(sets::is_dynamic), id_undefined);
    EXPECT_EQ(e.index_of(sets::tick_period), id_undefined);

    setup_log({});
  }

  TEST(engine, basic_entity_dynamic_1) {
    auto e = basic_entity { basic_entity::dynamic };

    EXPECT_EQ(e.get_count(), 2);
    EXPECT_NE(e.index_of(sets::is_dynamic), id_undefined);
    EXPECT_NE(e.index_of(sets::tick_period), id_undefined);

    EXPECT_TRUE(e.is_dynamic());
    EXPECT_EQ(e.get_tick_period(), basic_entity::default_tick_period);
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 1);
    EXPECT_EQ(e.get_by_id(sets::tick_period),
              basic_entity::default_tick_period);
  }

  TEST(engine, basic_entity_dynamic_2) {
    auto e = basic_entity { basic_entity::dynamic, 123 };

    EXPECT_EQ(e.get_count(), 2);
    EXPECT_NE(e.index_of(sets::is_dynamic), id_undefined);
    EXPECT_NE(e.index_of(sets::tick_period), id_undefined);

    EXPECT_TRUE(e.is_dynamic());
    EXPECT_EQ(e.get_tick_period(), 123);
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 1);
    EXPECT_EQ(e.get_by_id(sets::tick_period), 123);
  }

  TEST(engine, basic_entity_init) {
    setup_log([](std::string_view) {});

    auto e = my_entity {};
    e.do_init();

    EXPECT_EQ(e.get(e.index_of(sets::debug_value)), 123);
    EXPECT_EQ(e.bytes_get(0), 23);
    EXPECT_EQ(e.vec_get(0), 345);

    setup_log({});
  }

  TEST(engine, basic_entity_set_dynamic) {
    auto e = basic_entity { basic_entity::proto };

    e.set_dynamic(true);
    e.adjust();
    EXPECT_TRUE(e.is_dynamic());
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 1);

    e.set_dynamic(false);
    e.adjust();
    EXPECT_FALSE(e.is_dynamic());
    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 0);
  }

  TEST(engine, basic_entity_tick) {
    auto e = make_shared<basic_entity>();
    auto w = make_shared<world>();
    w->spawn(e, id_undefined);
    e->tick({ *w, engine::access::async });
  }

  TEST(engine, basic_entity_set_tick_period) {
    auto e = basic_entity { basic_entity::proto };

    e.set_tick_period(234);
    e.adjust();
    EXPECT_EQ(e.get_tick_period(), 234);
    EXPECT_EQ(e.get_by_id(sets::tick_period), 234);

    e.set_tick_period(345);
    e.adjust();
    EXPECT_EQ(e.get_tick_period(), 345);
    EXPECT_EQ(e.get_by_id(sets::tick_period), 345);
  }

  TEST(engine, basic_entity_get) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::proto };

    EXPECT_EQ(e.get(-1), 0);
    EXPECT_EQ(e.get(999), 0);

    EXPECT_EQ(e.get_by_id(sets::is_dynamic), 0);
    EXPECT_EQ(e.get_by_id(sets::tick_period),
              basic_entity::default_tick_period);

    setup_log({});
  }

  TEST(engine, basic_entity_set) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::proto };

    e.set(-1, 123);
    e.set(100, 123);
    e.set(e.index_of(sets::tick_period), 123);
    e.adjust();
    EXPECT_EQ(e.get_tick_period(), 123);

    setup_log({});
  }

  TEST(engine, basic_entity_apply_delta) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::proto };

    e.apply_delta(-1, 123);
    e.apply_delta(100, 123);
    e.apply_delta(e.index_of(sets::tick_period), 50);
    auto x = e.get_tick_period();
    e.adjust();
    EXPECT_EQ(e.get_tick_period(), x + 50);

    setup_log({});
  }

  TEST(engine, basic_entity_clock) {
    auto e = basic_entity { basic_entity::dynamic, 3 };

    EXPECT_TRUE(e.clock());
    e.reset_clock();
    EXPECT_FALSE(e.clock());
    EXPECT_FALSE(e.clock());
    EXPECT_TRUE(e.clock());
    EXPECT_FALSE(e.clock());
    EXPECT_FALSE(e.clock());
    EXPECT_TRUE(e.clock());
    e.set_clock(1);
    EXPECT_FALSE(e.clock());
    EXPECT_TRUE(e.clock());
  }

  TEST(engine, basic_endity_id_of) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::proto };

    EXPECT_EQ(e.id_of(e.index_of(sets::is_dynamic)),
              sets::is_dynamic);

    EXPECT_EQ(e.id_of(e.index_of(sets::tick_period)),
              sets::tick_period);

    EXPECT_EQ(e.index_of(-1), id_undefined);
    EXPECT_EQ(e.id_of(-1), id_undefined);
    EXPECT_EQ(e.id_of(999), id_undefined);

    setup_log({});
  }

  TEST(engine, basic_entity_scale_of) {
    setup_log([](std::string_view) {});

    auto e = basic_entity { basic_entity::proto };

    EXPECT_EQ(e.scale_of(e.index_of(sets::is_dynamic)), 1);

    EXPECT_EQ(e.scale_of(e.index_of(sets::tick_period)),
              sets::scale_time);

    EXPECT_EQ(e.scale_of(-1), 0);
    EXPECT_EQ(e.scale_of(999), 0);

    setup_log({});
  }

  TEST(engine, basic_entity_desync) {
    setup_log([](std::string_view) {});

    auto e = make_shared<my_entity>();
    auto w = make_shared<world>();

    EXPECT_EQ(e->get_id(), w->spawn(e, id_undefined));
    EXPECT_FALSE(w->is_desync());
    e->do_desync();
    EXPECT_TRUE(w->is_desync());

    setup_log({});
  }

  TEST(engine, basic_entity_self_destruct) {
    auto e = make_shared<my_entity>();
    auto w = make_shared<world>();

    w->spawn(e, id_undefined);
    e->do_self_destruct(*w);
    w->tick(1);
    EXPECT_EQ(e->get_id(), id_undefined);
  }

  TEST(engine, basic_entity_bytes_1) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};

    EXPECT_EQ(e.bytes_get_size(), 0);

    e.bytes_resize(10);
    EXPECT_EQ(e.bytes_get_size(), 10);

    e.bytes_set(2, 1);
    e.bytes_set(4, 2);
    e.bytes_set(6, 3);

    EXPECT_EQ(e.bytes_get(2), 0);
    EXPECT_EQ(e.bytes_get(4), 0);
    EXPECT_EQ(e.bytes_get(6), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(2), 1);
    EXPECT_EQ(e.bytes_get(4), 2);
    EXPECT_EQ(e.bytes_get(6), 3);

    EXPECT_EQ(e.bytes_get(-1), 0);
    EXPECT_EQ(e.bytes_get(10), 0);

    setup_log({});
  }

  TEST(engine, basic_entity_bytes_2) {
    auto e = basic_entity {};

    e.bytes_resize(10);
    e.bytes_set(5, 1);
    e.bytes_set(6, 2);
    e.bytes_set(7, 3);

    EXPECT_EQ(e.bytes_get(5), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(7), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(5), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(7), 3);
  }

  TEST(engine, basic_entity_bytes_3) {
    auto e = basic_entity {};

    e.bytes_resize(10);
    e.bytes_set(8, 4);
    e.bytes_set(7, 3);
    e.bytes_set(6, 2);
    e.bytes_set(1, 1);

    EXPECT_EQ(e.bytes_get(8), 0);
    EXPECT_EQ(e.bytes_get(7), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(1), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(1), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(7), 3);
    EXPECT_EQ(e.bytes_get(8), 4);
  }

  TEST(engine, basic_entity_bytes_4) {
    auto e = basic_entity {};

    e.bytes_resize(10);
    e.bytes_set(4, 1);
    e.bytes_set(6, 2);
    e.bytes_set(8, 3);

    EXPECT_EQ(e.bytes_get(4), 0);
    EXPECT_EQ(e.bytes_get(6), 0);
    EXPECT_EQ(e.bytes_get(8), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(4), 1);
    EXPECT_EQ(e.bytes_get(6), 2);
    EXPECT_EQ(e.bytes_get(8), 3);
  }

  TEST(engine, basic_entity_bytes_5) {
    auto e = basic_entity {};

    e.bytes_resize(10);
    e.bytes_set(5, 1);
    e.bytes_set(7, 2);
    e.bytes_set(6, 3);

    EXPECT_EQ(e.bytes_get(5), 0);
    EXPECT_EQ(e.bytes_get(7), 0);
    EXPECT_EQ(e.bytes_get(6), 0);

    e.adjust();

    EXPECT_EQ(e.bytes_get(5), 1);
    EXPECT_EQ(e.bytes_get(7), 2);
    EXPECT_EQ(e.bytes_get(6), 3);
  }

  TEST(engine, basic_entity_bytes_6) {
    auto e = basic_entity {};

    e.bytes_resize(3);
    e.bytes_set(0, 5);
    e.bytes_set(1, 6);
    e.bytes_set(2, 7);

    auto bytes = e.bytes_get_all();

    EXPECT_EQ(bytes.size(), 3);
    if (bytes.size() > 0)
      EXPECT_EQ(bytes[0], 0);
    if (bytes.size() > 1)
      EXPECT_EQ(bytes[1], 0);
    if (bytes.size() > 2)
      EXPECT_EQ(bytes[2], 0);

    e.adjust();
    bytes = e.bytes_get_all();

    EXPECT_EQ(bytes.size(), 3);
    if (bytes.size() > 0)
      EXPECT_EQ(bytes[0], 5);
    if (bytes.size() > 1)
      EXPECT_EQ(bytes[1], 6);
    if (bytes.size() > 2)
      EXPECT_EQ(bytes[2], 7);
  }

  TEST(engine, basic_entity_bytes_7) {
    auto e = basic_entity {};
    e.bytes_resize(5);
    e.bytes_set(0, 0);
    e.bytes_set(1, 1);
    e.bytes_set(2, 2);
    e.bytes_set(3, 3);
    e.bytes_set(4, 0);
    e.adjust();

    int8_t buf[3] = {};
    e.bytes_read(1, buf);

    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 3);
  }

  TEST(engine, basic_entity_bytes_8) {
    auto e = basic_entity {};
    e.bytes_resize(5);
    int8_t buf[3] = { 1, 2, 3 };
    e.bytes_write(1, buf);
    e.adjust();

    EXPECT_EQ(e.bytes_get(1), 1);
    EXPECT_EQ(e.bytes_get(2), 2);
    EXPECT_EQ(e.bytes_get(3), 3);
  }

  TEST(engine, basic_entity_bytes_9) {
    auto e = basic_entity {};
    e.bytes_resize(1);
    e.bytes_apply_delta(0, 2);
    e.adjust();
    e.bytes_apply_delta(0, 3);
    e.adjust();

    EXPECT_EQ(e.bytes_get(0), 5);
  }

  TEST(engine, basic_entity_bytes_10) {
    auto e = basic_entity {};
    e.bytes_resize(5);
    int8_t buf[3] = { 2, 2, 2 };
    int8_t app[3] = { 1, 2, 3 };
    e.bytes_write(1, buf);
    e.adjust();
    e.bytes_write_delta(1, app);
    e.adjust();

    EXPECT_EQ(e.bytes_get(1), 3);
    EXPECT_EQ(e.bytes_get(2), 4);
    EXPECT_EQ(e.bytes_get(3), 5);
  }

  TEST(engine, basic_entity_bytes_11) {
    auto e = basic_entity {};
    e.bytes_resize(5);
    int8_t buf[3] = { 2, 2, 2 };
    int8_t app[3] = { 1, 2, 3 };
    e.bytes_write(1, buf);
    e.adjust();
    e.bytes_erase_delta(1, app);
    e.adjust();

    EXPECT_EQ(e.bytes_get(1), 1);
    EXPECT_EQ(e.bytes_get(2), 0);
    EXPECT_EQ(e.bytes_get(3), -1);
  }

  TEST(engine, basic_entity_bytes_12) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};

    e.bytes_set(-1, 0);
    e.bytes_set(100, 0);
    e.bytes_apply_delta(-1, 0);
    e.bytes_apply_delta(100, 0);
    e.bytes_read(-1, {});
    e.bytes_read(100, {});
    e.bytes_write(-1, {});
    e.bytes_write(100, {});
    e.bytes_write_delta(-1, {});
    e.bytes_write_delta(100, {});
    e.bytes_erase_delta(-1, {});
    e.bytes_erase_delta(100, {});

    EXPECT_EQ(e.bytes_get(-1), 0);
    EXPECT_EQ(e.bytes_get(100), 0);

    setup_log({});
  }

  TEST(engine, basic_entity_bytes_13) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.bytes_resize(-1);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_1) {
    auto e = basic_entity {};
    e.vec_resize(5);
    EXPECT_EQ(e.vec_get_size(), 5);
  }

  TEST(engine, basic_entity_vec_2) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(1);
    e.vec_set(0, 5);
    e.adjust();

    EXPECT_EQ(e.vec_get(0), 5);
    EXPECT_EQ(e.vec_get(-1), 0);
    EXPECT_EQ(e.vec_get(999), 0);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_3) {
    auto e = basic_entity {};
    e.vec_resize(3);
    e.vec_set(0, 1);
    e.vec_set(1, 2);
    e.vec_set(2, 3);
    e.adjust();

    auto v = e.vec_get_all();
    EXPECT_EQ(v.size(), 3);
    if (v.size() > 0)
      EXPECT_EQ(v[0], 1);
    if (v.size() > 1)
      EXPECT_EQ(v[1], 2);
    if (v.size() > 2)
      EXPECT_EQ(v[2], 3);
  }

  TEST(engine, basic_entity_vec_4) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(3);
    e.vec_set(0, 1);
    e.vec_set(1, 2);
    e.vec_set(2, 3);
    e.vec_set(-1, 4);
    e.vec_set(999, 5);
    e.adjust();

    auto v = e.vec_get_all();
    EXPECT_EQ(v.size(), 3);
    if (v.size() > 0)
      EXPECT_EQ(v[0], 1);
    if (v.size() > 1)
      EXPECT_EQ(v[1], 2);
    if (v.size() > 2)
      EXPECT_EQ(v[2], 3);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_5) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(1);
    e.vec_apply_delta(0, 1);
    e.adjust();
    e.vec_apply_delta(0, 2);
    e.adjust();
    e.vec_apply_delta(-1, 3);
    e.vec_apply_delta(999, 4);
    e.adjust();

    EXPECT_EQ(e.vec_get(0), 3);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_6) {
    auto e = basic_entity {};
    e.vec_resize(5);
    e.vec_set(1, 1);
    e.vec_set(2, 2);
    e.vec_set(3, 3);
    e.adjust();

    intval buf[3] = {};
    e.vec_read(1, buf);

    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 3);
  }

  TEST(engine, basic_entity_vec_7) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(5);
    e.adjust();

    intval buf[3] = {};
    e.vec_read(-1, buf);
    e.vec_read(999, buf);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_8) {
    auto e = basic_entity {};
    e.vec_resize(5);
    intval buf[3] = { 1, 2, 3 };
    e.vec_write(1, buf);
    e.adjust();

    EXPECT_EQ(e.vec_get(1), 1);
    EXPECT_EQ(e.vec_get(2), 2);
    EXPECT_EQ(e.vec_get(3), 3);
  }

  TEST(engine, basic_entity_vec_9) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(5);
    intval buf[3] = { 1, 2, 3 };
    e.vec_write(-1, buf);
    e.vec_write(999, buf);
    e.adjust();

    setup_log({});
  }

  TEST(engine, basic_entity_vec_10) {
    auto e = basic_entity {};
    e.vec_resize(5);
    intval buf[3] = { 1, 2, 3 };
    e.vec_write(1, buf);
    e.adjust();

    EXPECT_EQ(e.vec_get(1), 1);
    EXPECT_EQ(e.vec_get(2), 2);
    EXPECT_EQ(e.vec_get(3), 3);
  }

  TEST(engine, basic_entity_vec_11) {
    auto e = basic_entity {};
    e.vec_resize(5);
    intval buf[3] = { 1, 2, 3 };
    e.vec_write(1, buf);
    e.adjust();
    intval app[3] = { 2, 3, 4 };
    e.vec_write_delta(1, app);
    e.adjust();

    EXPECT_EQ(e.vec_get(1), 3);
    EXPECT_EQ(e.vec_get(2), 5);
    EXPECT_EQ(e.vec_get(3), 7);
  }

  TEST(engine, basic_entity_vec_12) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(5);
    intval app[3] = { 2, 3, 4 };
    e.vec_write_delta(-1, app);
    e.vec_write_delta(999, app);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_13) {
    auto e = basic_entity {};
    e.vec_resize(5);
    intval buf[3] = { 3, 2, 3 };
    e.vec_write(1, buf);
    e.adjust();
    intval app[3] = { 2, 3, 4 };
    e.vec_erase_delta(1, app);
    e.adjust();

    EXPECT_EQ(e.vec_get(1), 1);
    EXPECT_EQ(e.vec_get(2), -1);
    EXPECT_EQ(e.vec_get(3), -1);
  }

  TEST(engine, basic_entity_vec_14) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(5);
    intval app[3] = { 2, 3, 4 };
    e.vec_erase_delta(-1, app);
    e.vec_erase_delta(999, app);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_15) {
    setup_log([](std::string_view) {});

    auto e = basic_entity {};
    e.vec_resize(-1);

    setup_log({});
  }

  TEST(engine, basic_entity_vec_add) {
    auto         e      = basic_entity {};
    intval const buf[3] = { 1, 2, 3 };
    e.vec_add(buf);

    EXPECT_EQ(e.vec_get(0), 1);
    EXPECT_EQ(e.vec_get(1), 2);
    EXPECT_EQ(e.vec_get(2), 3);
  }

  TEST(engine, basic_entity_multithreading) {
    auto obj     = make_shared<my_entity>();
    auto n_value = obj->index_of(sets::debug_value);

    auto fn_inc10 = [=]() {
      for (sl::index i = 0; i < 10; i++) obj->apply_delta(n_value, 1);
    };

    auto fn_inc100 = [=]() {
      for (sl::index i = 0; i < 100; i++)
        obj->apply_delta(n_value, 1);
    };

    auto threads_10 = sl::vector<thread>(10);

    for (auto &t : threads_10) t = thread(fn_inc10);
    for (auto &t : threads_10) t.join();

    obj->adjust();

    auto value1 = obj->get(n_value);

    EXPECT_EQ(value1, 100);

    auto threads_4 = sl::vector<thread>(4);

    for (auto &t : threads_4) t = thread(fn_inc100);
    for (auto &t : threads_4) t.join();

    obj->adjust();

    auto value2 = obj->get(n_value);

    EXPECT_EQ(value2, 500);
  }

  TEST(engine, basic_entity_copy_move) {
    auto foo = my_entity {};
    foo.set_tick_period(123);
    foo.adjust();
    auto bar = foo;
    EXPECT_EQ(bar.get_tick_period(), 123);
    foo.set_tick_period(234);
    foo.adjust();
    auto bus = std::move(foo);
    EXPECT_EQ(bus.get_tick_period(), 234);
    bar.set_tick_period(345);
    bar.adjust();
    foo = bar;
    EXPECT_EQ(foo.get_tick_period(), 345);
    bus.set_tick_period(456);
    bus.adjust();
    foo = std::move(bus);
    EXPECT_EQ(foo.get_tick_period(), 456);
  }

  TEST(engine, basic_entity_deadlocks) {
    setup_log([](std::string_view) {});

    auto e = my_entity {};
    e.lock();

    e.set_dynamic(false);
    e.set_tick_period(0);
    e.set_clock(0);
    e.reset_clock();
    e.set(0, 0);
    e.apply_delta(0, 0);

    e.bytes_read(0, {});
    e.bytes_set(0, 0);
    e.bytes_write(0, {});
    e.bytes_apply_delta(0, 0);
    e.bytes_write_delta(0, {});
    e.bytes_erase_delta(0, {});
    e.bytes_resize(0);

    e.vec_read(0, {});
    e.vec_set(0, 0);
    e.vec_write(0, {});
    e.vec_apply_delta(0, 0);
    e.vec_write_delta(0, {});
    e.vec_erase_delta(0, {});
    e.vec_resize(0);

    e.vec_add({});
    e.vec_add_sorted({});
    e.vec_insert(0, {});
    e.vec_erase(0, 0);
    e.vec_erase_by_value(0, 0);
    e.vec_erase_by_value_sorted(0, 0);

    e.adjust();

    EXPECT_FALSE(e.clock());
    EXPECT_EQ(e.get(0), 0);
    EXPECT_EQ(e.bytes_get_size(), 0);
    EXPECT_EQ(e.bytes_get(0), 0);
    EXPECT_TRUE(e.bytes_get_all().empty());
    EXPECT_EQ(e.vec_get_size(), 0);
    EXPECT_EQ(e.vec_get(0), 0);
    EXPECT_TRUE(e.vec_get_all().empty());
    EXPECT_FALSE(e.is_dynamic());
    EXPECT_EQ(e.get_tick_period(), 0);

    e.unlock();

    setup_log({});
  }
}
