/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/solver.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::solver, std::make_unique, engine::basic_impact,
      engine::ptr_impact, std::string_view;

  TEST(engine, solver_blank) {
    auto sol = solver {};
    EXPECT_EQ(sol.get_time(), 0);
    EXPECT_EQ(sol.get_position(), 0);
    EXPECT_EQ(sol.get_seed(), 0);
    EXPECT_TRUE(sol.get_history().empty());
  }

  TEST(engine, solver_no_event) {
    auto sol             = solver {};
    bool event_performed = false;
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) { event_performed = true; });
    sol.apply({});
    sol.schedule(1);
    EXPECT_FALSE(event_performed);
    EXPECT_EQ(sol.get_time(), 1);
    EXPECT_EQ(sol.get_position(), 0);
  }

  TEST(engine, solver_one_event) {
    auto sol          = solver {};
    bool event_queued = false;
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) { event_queued = true; });
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      0, 0, 0, 0, 0, 0, 0, 0 });
    sol.schedule(1);
    EXPECT_TRUE(event_queued);
    EXPECT_EQ(sol.get_time(), 1);
    EXPECT_EQ(sol.get_position(), 1);
  }

  TEST(engine, solver_invalid_event_time) {
    auto sol          = solver {};
    bool event_queued = false;
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) { event_queued = true; });
    sol.schedule(1);
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      0, 0, 0, 0, 0, 0, 0, 0 });
    sol.schedule(1);
    EXPECT_FALSE(event_queued);
    EXPECT_EQ(sol.get_time(), 2);
    EXPECT_EQ(sol.get_position(), 0);
  }

  TEST(engine, solver_future_event) {
    auto sol          = solver {};
    bool event_queued = false;
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) { event_queued = true; });
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      2, 0, 0, 0, 0, 0, 0, 0 });
    sol.schedule(2);
    EXPECT_FALSE(event_queued);
    EXPECT_EQ(sol.get_time(), 2);
    EXPECT_EQ(sol.get_position(), 0);
    sol.schedule(1);
    EXPECT_TRUE(event_queued);
    EXPECT_EQ(sol.get_time(), 3);
    EXPECT_EQ(sol.get_position(), 1);
  }

  TEST(engine, solver_schedule) {
    auto sol          = solver {};
    auto time_elapsed = sl::time {};
    sol.on_schedule([&](sl::time delta) { time_elapsed += delta; });
    sol.schedule(1);
    EXPECT_EQ(time_elapsed, 1);
    EXPECT_EQ(sol.get_time(), 1);
    EXPECT_EQ(sol.get_position(), 0);
  }

  TEST(engine, solver_join_before_queue) {
    auto sol                = solver {};
    auto join_called        = sl::whole {};
    auto event_queued_after = false;
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) {
      if (join_called > 0)
        event_queued_after = true;
    });
    sol.on_join([&]() { join_called++; });
    sol.schedule(1);
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      1, 0, 0, 0, 0, 0, 0, 0 });
    sol.schedule(1);
    EXPECT_EQ(join_called, 1);
    EXPECT_TRUE(event_queued_after);
    EXPECT_EQ(sol.get_time(), 2);
    EXPECT_EQ(sol.get_position(), 1);
  }

  TEST(engine, solver_schedule_invalid_time) {
    auto sol          = solver {};
    auto time_elapsed = sl::time {};
    sol.on_schedule([&](sl::time delta) { time_elapsed += delta; });
    sol.schedule(1);
    EXPECT_EQ(time_elapsed, 1);
    EXPECT_EQ(sol.get_time(), 1);
    sol.schedule(-1);
    EXPECT_EQ(time_elapsed, 1);
    EXPECT_EQ(sol.get_time(), 1);
    sol.schedule(0);
    EXPECT_EQ(time_elapsed, 1);
    EXPECT_EQ(sol.get_time(), 1);
  }

  TEST(engine, solver_seed) {
    auto sol      = solver {};
    auto seed_set = sl::whole {};
    sol.on_seed([&](engine::seed_type) { seed_set++; });
    sol.schedule(1);
    EXPECT_EQ(seed_set, 1);
  }

  TEST(engine, solver_rewind_blank) {
    auto sol = solver {};
    sol.rewind_to(0);
    EXPECT_EQ(sol.get_time(), 0);
  }

  TEST(engine, solver_rewind) {
    auto sol          = solver {};
    auto reset_called = sl::whole {};
    auto seed_set     = sl::whole {};
    auto event_queued = sl::whole {};
    sol.on_reset([&]() { reset_called++; });
    sol.on_seed([&](engine::seed_type) { seed_set++; });
    sol.on_decode(
        [](span_cbyte) { return make_unique<basic_impact>(); });
    sol.on_queue([&](ptr_impact) { event_queued++; });
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      0, 0, 0, 0, 0, 0, 0, 0 });
    EXPECT_EQ(reset_called, 0);
    EXPECT_EQ(seed_set, 0);
    EXPECT_EQ(event_queued, 0);
    EXPECT_EQ(sol.get_time(), 0);
    EXPECT_EQ(sol.get_position(), 0);
    sol.schedule(1);
    EXPECT_EQ(reset_called, 0);
    EXPECT_EQ(seed_set, 1);
    EXPECT_EQ(event_queued, 1);
    EXPECT_EQ(sol.get_time(), 1);
    EXPECT_EQ(sol.get_position(), 1);
    sol.rewind_to(0);
    EXPECT_EQ(reset_called, 1);
    EXPECT_EQ(seed_set, 1);
    EXPECT_EQ(event_queued, 1);
    EXPECT_EQ(sol.get_time(), 0);
    EXPECT_EQ(sol.get_position(), 0);
    sol.schedule(1);
    EXPECT_EQ(reset_called, 1);
    EXPECT_EQ(seed_set, 2);
    EXPECT_EQ(event_queued, 2);
    EXPECT_EQ(sol.get_time(), 1);
    EXPECT_EQ(sol.get_position(), 1);
  }

  TEST(engine, solver_tick) {
    auto sol          = solver {};
    bool tick_called  = false;
    auto time_elapsed = sl::whole {};
    sol.on_schedule([&](sl::time delta) { time_elapsed += delta; });
    sol.on_tick([&](sl::time delta) {
      tick_called = true;
      time_elapsed += delta;
    });
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      0, 0, 0, 0, 0, 0, 0, 0 });
    sol.apply(vbyte { 0, 0,                   //
                      0, 0, 0, 0, 0, 0, 0, 0, //
                      9, 0, 0, 0, 0, 0, 0, 0 });
    sol.schedule(10);
    EXPECT_TRUE(tick_called);
    EXPECT_EQ(time_elapsed, 10);
    EXPECT_EQ(sol.get_time(), 10);
    EXPECT_EQ(sol.get_position(), 2);
  }
}
