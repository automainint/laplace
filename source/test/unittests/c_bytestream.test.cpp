/*  test/unittests/c_profiler.test.cpp
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

#include "../../laplace/core/bytestream.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, bytestream_get) {
    uint8_t const bytes[] = { 1, 2, 3, 4, 5 };

    auto in = ibytestream { bytes };

    EXPECT_EQ(in.get(), 1);
    EXPECT_EQ(in.get(), 2);
    EXPECT_EQ(in.get(), 3);
    EXPECT_EQ(in.get(), 4);
    EXPECT_EQ(in.get(), 5);
  }

  TEST(core, bytestream_seek_pos) {
    uint8_t const bytes[] = { 1, 2, 3, 4, 5 };

    auto in = ibytestream { bytes };

    in.seekg(3);
    EXPECT_EQ(in.get(), 4);
    EXPECT_EQ(in.get(), 5);
  }

  TEST(core, bytestream_seek_beg) {
    uint8_t const bytes[] = { 1, 2, 3, 4, 5 };

    auto in = ibytestream { bytes };

    in.seekg(3, in.beg);
    EXPECT_EQ(in.get(), 4);
    EXPECT_EQ(in.get(), 5);
  }

  TEST(core, bytestream_seek_end) {
    uint8_t const bytes[] = { 1, 2, 3, 4, 5 };

    auto in = ibytestream { bytes };

    in.seekg(-3, in.end);
    EXPECT_EQ(in.get(), 3);
    EXPECT_EQ(in.get(), 4);
    EXPECT_EQ(in.get(), 5);
  }

  TEST(core, bytestream_seek_cur) {
    uint8_t const bytes[] = { 1, 2, 3, 4, 5 };

    auto in = ibytestream { bytes };

    EXPECT_EQ(in.get(), 1);
    in.seekg(3, in.cur);
    EXPECT_EQ(in.get(), 5);
  }
}
