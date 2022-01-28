/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/string.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using std::string_view;

  TEST(core, string_hex_to_int) {
    setup_log([](string_view) {});

    EXPECT_EQ(hex_to_int('X'), 0);
    EXPECT_EQ(hex_to_int('0'), 0);
    EXPECT_EQ(hex_to_int('1'), 1);
    EXPECT_EQ(hex_to_int('2'), 2);
    EXPECT_EQ(hex_to_int('3'), 3);
    EXPECT_EQ(hex_to_int('4'), 4);
    EXPECT_EQ(hex_to_int('5'), 5);
    EXPECT_EQ(hex_to_int('6'), 6);
    EXPECT_EQ(hex_to_int('7'), 7);
    EXPECT_EQ(hex_to_int('8'), 8);
    EXPECT_EQ(hex_to_int('9'), 9);
    EXPECT_EQ(hex_to_int('a'), 10);
    EXPECT_EQ(hex_to_int('b'), 11);
    EXPECT_EQ(hex_to_int('c'), 12);
    EXPECT_EQ(hex_to_int('d'), 13);
    EXPECT_EQ(hex_to_int('e'), 14);
    EXPECT_EQ(hex_to_int('f'), 15);
    EXPECT_EQ(hex_to_int('A'), 10);
    EXPECT_EQ(hex_to_int('B'), 11);
    EXPECT_EQ(hex_to_int('C'), 12);
    EXPECT_EQ(hex_to_int('D'), 13);
    EXPECT_EQ(hex_to_int('E'), 14);
    EXPECT_EQ(hex_to_int('F'), 15);

    EXPECT_EQ(hex_to_int(u'X'), 0);
    EXPECT_EQ(hex_to_int(u'0'), 0);
    EXPECT_EQ(hex_to_int(u'1'), 1);
    EXPECT_EQ(hex_to_int(u'2'), 2);
    EXPECT_EQ(hex_to_int(u'3'), 3);
    EXPECT_EQ(hex_to_int(u'4'), 4);
    EXPECT_EQ(hex_to_int(u'5'), 5);
    EXPECT_EQ(hex_to_int(u'6'), 6);
    EXPECT_EQ(hex_to_int(u'7'), 7);
    EXPECT_EQ(hex_to_int(u'8'), 8);
    EXPECT_EQ(hex_to_int(u'9'), 9);
    EXPECT_EQ(hex_to_int(u'a'), 10);
    EXPECT_EQ(hex_to_int(u'b'), 11);
    EXPECT_EQ(hex_to_int(u'c'), 12);
    EXPECT_EQ(hex_to_int(u'd'), 13);
    EXPECT_EQ(hex_to_int(u'e'), 14);
    EXPECT_EQ(hex_to_int(u'f'), 15);
    EXPECT_EQ(hex_to_int(u'A'), 10);
    EXPECT_EQ(hex_to_int(u'B'), 11);
    EXPECT_EQ(hex_to_int(u'C'), 12);
    EXPECT_EQ(hex_to_int(u'D'), 13);
    EXPECT_EQ(hex_to_int(u'E'), 14);
    EXPECT_EQ(hex_to_int(u'F'), 15);

    EXPECT_EQ(hex_to_int(U'X'), 0);
    EXPECT_EQ(hex_to_int(U'0'), 0);
    EXPECT_EQ(hex_to_int(U'1'), 1);
    EXPECT_EQ(hex_to_int(U'2'), 2);
    EXPECT_EQ(hex_to_int(U'3'), 3);
    EXPECT_EQ(hex_to_int(U'4'), 4);
    EXPECT_EQ(hex_to_int(U'5'), 5);
    EXPECT_EQ(hex_to_int(U'6'), 6);
    EXPECT_EQ(hex_to_int(U'7'), 7);
    EXPECT_EQ(hex_to_int(U'8'), 8);
    EXPECT_EQ(hex_to_int(U'9'), 9);
    EXPECT_EQ(hex_to_int(U'a'), 10);
    EXPECT_EQ(hex_to_int(U'b'), 11);
    EXPECT_EQ(hex_to_int(U'c'), 12);
    EXPECT_EQ(hex_to_int(U'd'), 13);
    EXPECT_EQ(hex_to_int(U'e'), 14);
    EXPECT_EQ(hex_to_int(U'f'), 15);
    EXPECT_EQ(hex_to_int(U'A'), 10);
    EXPECT_EQ(hex_to_int(U'B'), 11);
    EXPECT_EQ(hex_to_int(U'C'), 12);
    EXPECT_EQ(hex_to_int(U'D'), 13);
    EXPECT_EQ(hex_to_int(U'E'), 14);
    EXPECT_EQ(hex_to_int(U'F'), 15);

    setup_log({});
  }

  TEST(core, string_int_to_hex) {
    setup_log([](string_view) {});

    for (int i = 0; i <= 15; i++)
      EXPECT_EQ(hex_to_int(int_to_hex(i)), i);

    EXPECT_EQ(int_to_hex(-1), '\0');
    EXPECT_EQ(int_to_hex(16), '\0');
    EXPECT_EQ(int_to_hex(17), '\0');

    setup_log({});
  }

  TEST(core, string_fmt) {
    EXPECT_EQ(fmt("foo bar"), "foo bar");
    EXPECT_EQ(fmt(u8"foo bar"), u8"foo bar");
  }

  TEST(core, string_convertion) {
    EXPECT_EQ(as_ascii_string(u8"foo bar"), "foo bar");
    EXPECT_EQ(as_u8string("foo bar"), u8"foo bar");

    EXPECT_EQ(to_string(L"foo bar"), "foo bar");
    EXPECT_EQ(to_string(u8"foo bar"), "foo bar");
    EXPECT_EQ(to_wstring(u8"foo bar"), L"foo bar");
  }

  TEST(core, string_to_integer) {
    EXPECT_EQ(to_integer("foo bar"), 0);
    EXPECT_EQ(to_integer("0"), 0);
    EXPECT_EQ(to_integer("-1"), -1);
    EXPECT_EQ(to_integer("42"), 42);

    EXPECT_EQ(to_integer(u8"foo bar"), 0);
    EXPECT_EQ(to_integer(u8"0"), 0);
    EXPECT_EQ(to_integer(u8"-1"), -1);
    EXPECT_EQ(to_integer(u8"42"), 42);
  }

  TEST(core, string_to_uint) {
    EXPECT_EQ(to_uint("foo bar"), 0);
    EXPECT_EQ(to_uint("0"), 0);
    EXPECT_EQ(to_uint("-1"), -1);
    EXPECT_EQ(to_uint("42"), 42);

    EXPECT_EQ(to_uint(u8"foo bar"), 0);
    EXPECT_EQ(to_uint(u8"0"), 0);
    EXPECT_EQ(to_uint(u8"-1"), -1);
    EXPECT_EQ(to_uint(u8"42"), 42);
  }

  TEST(core, string_to_double) {
    EXPECT_DOUBLE_EQ(to_real("foo bar"), 0.);
    EXPECT_DOUBLE_EQ(to_real("0"), 0.);
    EXPECT_DOUBLE_EQ(to_real("-1"), -1.);
    EXPECT_DOUBLE_EQ(to_real("3.14"), 3.14);

    EXPECT_DOUBLE_EQ(to_real(u8"foo bar"), 0.);
    EXPECT_DOUBLE_EQ(to_real(u8"0"), 0.);
    EXPECT_DOUBLE_EQ(to_real(u8"-1"), -1.);
    EXPECT_DOUBLE_EQ(to_real(u8"3.14"), 3.14);
  }
}
