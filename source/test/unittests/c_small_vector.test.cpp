/*  test/unittests/c_small_vector.test.cpp
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

#include "../../laplace/core/small_vector.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, small_vector_empty_at_construct) {
    auto v = small_vector<int> {};
    EXPECT_TRUE(v.empty());
  }

  TEST(core, small_vector_size_at_construct) {
    auto v = small_vector<int> {};
    EXPECT_EQ(v.size(), 0);
  }

  TEST(core, small_vector_push_back_not_empty) {
    auto v = small_vector<int> {};
    v.push_back(0);
    EXPECT_FALSE(v.empty());
  }

  TEST(core, small_vector_push_back_size) {
    auto v = small_vector<int> {};
    v.push_back(0);
    EXPECT_EQ(v.size(), 1);
  }

  TEST(core, small_vector_push_back_capacity) {
    auto v = small_vector<int> {};
    v.push_back(0);
    EXPECT_GE(v.capacity(), 1);
  }

  TEST(core, small_vector_resize_size) {
    auto v = small_vector<int> {};
    v.resize(100);
    EXPECT_EQ(v.size(), 100);
  }

  TEST(core, small_vector_resize_capacity) {
    auto v = small_vector<int> {};
    v.resize(100);
    EXPECT_GE(v.capacity(), 100);
  }

  TEST(core, small_vector_push_back_at) {
    auto v = small_vector<int> {};
    v.push_back(10);
    EXPECT_EQ(v.at(0), 10);
  }

  TEST(core, small_vector_at_set_get) {
    auto v = small_vector<int> {};
    v.resize(1);
    v.at(0) = 3;
    EXPECT_EQ(v.at(0), 3);
  }

  TEST(core, small_vector_push_back_twice) {
    auto v = small_vector<int> {};
    v.push_back(10);
    v.push_back(20);
    EXPECT_EQ(v.at(0), 10);
    EXPECT_EQ(v.at(1), 20);
  }

  TEST(core, small_vector_resize_big) {
    auto v = small_vector<int> {};
    v.resize(500);
    v.at(499) = 42;
    EXPECT_EQ(v.at(499), 42);
  }

  TEST(core, small_vector_move) {
    auto v = []() {
      auto v = small_vector<int> {};
      v.push_back(1);
      return v;
    }();

    EXPECT_EQ(v.at(0), 1);
  }

  TEST(core, small_vector_move_big) {
    auto v = []() {
      auto v = small_vector<int> {};
      v.resize(500);
      v.at(499) = 5;
      return v;
    }();

    EXPECT_EQ(v.at(499), 5);
  }

  TEST(core, small_vector_resize_twice) {
    auto v = small_vector<int> {};
    v.resize(10);
    v.at(9) = 1;
    v.resize(200);
    v.at(199) = 2;
    EXPECT_EQ(v.at(9), 1);
    EXPECT_EQ(v.at(199), 2);
  }

  TEST(core, small_vector_copy) {
    auto v = small_vector<int> {};
    v.push_back(9);
    auto v2 = small_vector<int> { v };
    EXPECT_EQ(v2.at(0), 9);
  }

  TEST(core, small_vector_copy_big) {
    auto v = small_vector<int> {};
    v.resize(400);
    v.at(399) = 39;
    auto v2   = small_vector<int> { v };
    EXPECT_EQ(v2.at(399), 39);
  }

  TEST(core, small_vector_iterator) {
    auto v = small_vector<int> {};
    v.resize(100);

    int k = 0;
    for (auto &x : v) { x = ++k; }

    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(99), 100);
  }

  TEST(core, small_vector_assign) {
    auto v  = small_vector<int> {};
    auto v2 = small_vector<int> {};
    v2.push_back(1);
    v2.push_back(5);
    v2.push_back(2);
    v2.push_back(6);
    v = v2;
    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(1), 5);
    EXPECT_EQ(v.at(2), 2);
    EXPECT_EQ(v.at(3), 6);
  }

  TEST(core, small_vector_assign_big) {
    auto v  = small_vector<int> {};
    auto v2 = small_vector<int> {};

    v2.resize(400);
    v2.at(350) = 10;
    v2.at(398) = 4;
    v2.at(399) = 50;

    v = v2;

    EXPECT_EQ(v.at(350), 10);
    EXPECT_EQ(v.at(398), 4);
    EXPECT_EQ(v.at(399), 50);
  }

  TEST(core, small_vector_assign_move) {
    auto v = small_vector<int> {};

    v = []() {
      auto v = small_vector<int> {};
      v.push_back(1);
      v.push_back(5);
      v.push_back(2);
      v.push_back(6);
      return v;
    }();

    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(1), 5);
    EXPECT_EQ(v.at(2), 2);
    EXPECT_EQ(v.at(3), 6);
  }

  TEST(core, small_vector_assign_move_big) {
    auto v = small_vector<int> {};

    v = []() {
      auto v = small_vector<int> {};
      v.resize(400);
      v.at(350) = 10;
      v.at(398) = 4;
      v.at(399) = 50;
      return v;
    }();

    EXPECT_EQ(v.at(350), 10);
    EXPECT_EQ(v.at(398), 4);
    EXPECT_EQ(v.at(399), 50);
  }

  TEST(core, small_vector_front) {
    auto v = small_vector<int> {};
    v.push_back(14);
    v.push_back(15);
    EXPECT_EQ(v.front(), 14);
  }

  TEST(core, small_vector_back) {
    auto v = small_vector<int> {};
    v.push_back(14);
    v.push_back(15);
    EXPECT_EQ(v.back(), 15);
  }

  TEST(core, small_vector_reserve) {
    auto v = small_vector<int> {};
    v.reserve(100);
    EXPECT_GE(v.capacity(), 100);
  }

  TEST(core, small_vector_clear) {
    auto v = small_vector<int> {};
    v.push_back(0);
    v.clear();
    EXPECT_TRUE(v.empty());
  }

  TEST(core, small_vector_clear_big) {
    auto v = small_vector<int> {};
    v.resize(400);
    v.clear();
    EXPECT_TRUE(v.empty());
  }

  TEST(core, small_vector_insert) {
    auto v = small_vector<int> {};
    v.resize(100, 1);
    v.insert(v.begin() + 5, 10);
    EXPECT_EQ(v.at(5), 10);
  }

  TEST(core, small_vector_emplace) {
    auto v = small_vector<int> {};
    v.resize(100, 1);
    v.emplace(v.begin() + 5, 10);
    EXPECT_EQ(v.at(5), 10);
  }

  TEST(core, small_vector_emplace_back) {
    auto v = small_vector<int> {};
    v.resize(100, 1);
    v.emplace_back(10);
    EXPECT_EQ(v.at(100), 10);
  }

  TEST(core, small_vector_compare) {
    auto a = small_vector<int> {};
    a.resize(5, 2);
    auto b = small_vector<int> {};
    b.resize(5, 1);
    b.at(0) = 3;
    auto c  = small_vector<int> { b };
    c.at(4) = 0;
    EXPECT_TRUE(a.compare(a) == 0);
    EXPECT_TRUE(a.compare(b) < 0);
    EXPECT_TRUE(a.compare(c) < 0);
    EXPECT_TRUE(b.compare(c) > 0);
  }

  TEST(core, small_vector_init_list) {
    auto v = small_vector<int> { 1, 2, 3, 4, 5 };
    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(1), 2);
    EXPECT_EQ(v.at(2), 3);
    EXPECT_EQ(v.at(3), 4);
    EXPECT_EQ(v.at(4), 5);
  }

  TEST(core, small_vector_construct) {
    auto v = small_vector<int>(10, 2);
    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(v[5], 2);
  }

  TEST(core, small_vector_access_operator) {
    auto v = small_vector<int> { 1, 2, 3, 4, 5 };
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(v[4], 5);
  }

  TEST(core, small_vector_compare_operator) {
    auto a = small_vector<int> { 1, 2, 3 };
    auto b = small_vector<int> { 1, 2, 4 };
    auto c = small_vector<int> { 1, 2, 2 };
    auto d = small_vector<int> { 2, 2 };
    auto e = small_vector<int> { 1, 2, 3, 1 };

    EXPECT_TRUE(a == a);
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a >= c);
    EXPECT_TRUE(a > c);
    EXPECT_TRUE(a < d);
    EXPECT_TRUE(a < e);
    EXPECT_TRUE(b > c);
    EXPECT_TRUE(b < d);
    EXPECT_TRUE(b > e);
    EXPECT_TRUE(c < d);
    EXPECT_TRUE(c < e);
    EXPECT_TRUE(d > e);
  }
}
