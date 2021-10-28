/*  test/unittests/c_utf8.test.cpp
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

#include "../../laplace/core/utf8.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using std::u8string;

  TEST(core, utf8_length) {
    EXPECT_EQ(utf8::length(u8""), 0);
    EXPECT_EQ(utf8::length(u8"foo bar"), 7);
  }

  TEST(core, utf8_decode) {
    auto const s = u8string { u8"foo bar" };
    auto       n = sl::index {};
    auto       c = char32_t {};

    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'f');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'o');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'o');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U' ');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'b');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'a');
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, U'r');
    EXPECT_FALSE(utf8::decode(s, n, c));

    n = -1;
    EXPECT_FALSE(utf8::decode(s, n, c));
    n = 8;
    EXPECT_FALSE(utf8::decode(s, n, c));
  }

  TEST(core, utf8_encode_and_decode) {
    auto s = u8string {};
    auto n = sl::index {};

    n = -1;
    EXPECT_FALSE(utf8::encode('x', s, n));
    n = 2;
    EXPECT_FALSE(utf8::encode('x', s, n));

    n = 0;
    EXPECT_TRUE(utf8::encode(100, s, n));
    EXPECT_TRUE(utf8::encode(1000, s, n));
    EXPECT_TRUE(utf8::encode(10000, s, n));
    EXPECT_TRUE(utf8::encode(100000, s, n));
    EXPECT_FALSE(utf8::encode(0x20FFFF, s, n));

    auto c = char32_t {};
    n      = 0;
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, 100);
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, 1000);
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, 10000);
    EXPECT_TRUE(utf8::decode(s, n, c));
    EXPECT_EQ(c, 100000);
  }

  TEST(core, utf8_decode_invalid) {
    auto n = sl::index {};
    auto c = char32_t {};

    EXPECT_FALSE(utf8::decode(u8"\xff", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xf8", n, c));
    EXPECT_FALSE(utf8::decode(u8"\x80", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xc0", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xc0\xc0", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xe0", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xe0\xc0\x80", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xe0\x80\xc0", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xf0", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xf0\xc0\x80\x80", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xf0\x80\xc0\x80", n, c));
    EXPECT_FALSE(utf8::decode(u8"\xf0\x80\x80\xc0", n, c));
  }
}
