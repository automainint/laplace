/*  test/unittests/c_utils.test.cpp
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

#include "../../laplace/core/serial.h"
#include "../../laplace/core/utils.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using serial::rd, serial::wr, serial::write_bytes;

  TEST(core, utils_read) {
    const vbyte a { 0xff, 0x10, 0x30 };

    EXPECT_EQ(rd<uint8_t>(a, 0), 0xff);
    EXPECT_EQ(rd<uint8_t>(a, 1), 0x10);
    EXPECT_EQ(rd<uint8_t>(a, 2), 0x30);

    EXPECT_EQ(rd<uint16_t>(a, 0), 0x10ff);
    EXPECT_EQ(rd<uint16_t>(a, 1), 0x3010);
  }

  TEST(core, utils_write) {
    vbyte a { 0, 0, 0 };

    wr<uint8_t>(a, 0, 0xff);
    wr<uint16_t>(a, 1, 0x3010);

    EXPECT_EQ(rd<uint8_t>(a, 0), 0xff);
    EXPECT_EQ(rd<uint8_t>(a, 1), 0x10);
    EXPECT_EQ(rd<uint8_t>(a, 2), 0x30);

    EXPECT_EQ(rd<uint16_t>(a, 0), 0x10ff);
    EXPECT_EQ(rd<uint16_t>(a, 1), 0x3010);
  }

  TEST(core, utils_identefier) {
    if constexpr (sizeof(sl::index) == 8) {
      uint64_t a = 0x1020304050607080;
      uint64_t b = 0x2030405060708090;

      EXPECT_EQ(as_index(a), 0x1020304050607080ull);
      EXPECT_EQ(as_index(b), 0x2030405060708090ull);
    }

    if constexpr (sizeof(sl::index) == 4) {
      uint64_t a = 0x10203040;
      uint64_t b = 0x20304050;

      EXPECT_EQ(as_index(a), 0x10203040u);
      EXPECT_EQ(as_index(b), 0x20304050u);
    }
  }

  TEST(core, utils_write_bytes) {
    std::array<uint8_t, 16> data  = {};
    std::array<uint8_t, 8>  bytes = { 1, 2, 3, 4, 5, 6, 7, 8 };

    write_bytes(data, int32_t(123), int32_t(-1),
                std::span<const uint8_t>(bytes.begin(), bytes.end()));

    EXPECT_EQ(rd<int32_t>(data, 0), 123);
    EXPECT_EQ(rd<int32_t>(data, 4), -1);
    EXPECT_EQ(rd<uint8_t>(data, 8), 1);
    EXPECT_EQ(rd<uint8_t>(data, 9), 2);
    EXPECT_EQ(rd<uint8_t>(data, 10), 3);
    EXPECT_EQ(rd<uint8_t>(data, 11), 4);
    EXPECT_EQ(rd<uint8_t>(data, 12), 5);
    EXPECT_EQ(rd<uint8_t>(data, 13), 6);
    EXPECT_EQ(rd<uint8_t>(data, 14), 7);
    EXPECT_EQ(rd<uint8_t>(data, 15), 8);
  }
}
