/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/clock.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::clock, std::string_view;

  TEST(network, clock_defaults) {
    auto clo = clock {};
    EXPECT_EQ(clo.get_tick_duration(), clock::default_tick_duration);
    EXPECT_EQ(clo.get_latency(), clock::default_latency);
    EXPECT_EQ(clo.get_overtake_factor(),
              clock::default_overtake_factor);
  }

  TEST(network, clock_options) {
    auto clo = clock {};
    clo.set_tick_duration(5);
    clo.set_latency(20);
    clo.set_overtake_factor(4);
    EXPECT_EQ(clo.get_tick_duration(), 5);
    EXPECT_EQ(clo.get_latency(), 20);
    EXPECT_EQ(clo.get_overtake_factor(), 4);
  }

  TEST(network, clock_options_invalid) {
    disable_log();
    auto clo = clock {};
    clo.set_tick_duration(0);
    clo.set_latency(-1);
    clo.set_overtake_factor(0);
    EXPECT_EQ(clo.get_tick_duration(), clock::default_tick_duration);
    EXPECT_EQ(clo.get_latency(), clock::default_latency);
    EXPECT_EQ(clo.get_overtake_factor(),
              clock::default_overtake_factor);
    setup_log({});
  }

  TEST(network, clock_time_overflow) {
    auto clo = clock {};
    EXPECT_FALSE(clo.is_overflow());
    clo.tick(0xffffffffffffffff);
    clo.tick(0xffffffffffffffff);
    EXPECT_TRUE(clo.is_overflow());
  }

  TEST(network, clock_tick_duration) {
    auto clo = clock {};
    clo.set_tick_duration(100);
    EXPECT_EQ(clo.get_time(), 0);
    clo.tick(99);
    EXPECT_EQ(clo.get_time(), 0);
    clo.tick(1);
    EXPECT_EQ(clo.get_time(), 1);
    clo.tick(101);
    EXPECT_EQ(clo.get_time(), 2);
    clo.tick(99);
    EXPECT_EQ(clo.get_time(), 3);
  }

  TEST(network, clock_master_time) {
    auto clo = clock {};
    clo.set_tick_duration(10);
    clo.set_master_time(10);
    clo.tick(200);
    EXPECT_EQ(clo.get_time(), 10);
  }

  TEST(network, clock_local_time) {
    auto clo = clock {};
    EXPECT_EQ(clo.get_local_time(), 0);
    clo.tick(10);
    EXPECT_EQ(clo.get_local_time(), 10);
    clo.tick(50);
    EXPECT_EQ(clo.get_local_time(), 60);
  }

  TEST(network, clock_time_elapsed) {
    auto clo = clock {};
    clo.set_tick_duration(10);
    clo.tick(100);
    EXPECT_EQ(clo.get_time_elapsed(), 10);
    clo.tick(59);
    EXPECT_EQ(clo.get_time_elapsed(), 5);
    clo.tick(201);
    EXPECT_EQ(clo.get_time_elapsed(), 21);
  }

  TEST(network, clock_overtake_1) {
    auto clo = clock {};
    clo.set_tick_duration(10);
    clo.set_latency(0);
    clo.set_master_time(10);
    clo.tick(1);
    EXPECT_EQ(clo.get_time(), 10);
  }

  TEST(network, clock_overtake_2) {
    auto clo = clock {};
    clo.set_tick_duration(10);
    clo.set_latency(50);
    clo.set_overtake_factor(1);
    clo.set_master_time(10);
    clo.tick(1);
    EXPECT_EQ(clo.get_time(), 5);
  }
}
