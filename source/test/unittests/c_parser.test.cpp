/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/parser.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::parser, std::u8string;

  TEST(core, parser_string) {
    auto const s = "test";

    auto p = parser::wrap(s);
    auto x = u8string {};

    EXPECT_TRUE(p.parse(" %s ", &x));
    EXPECT_EQ(x, u8"test");
  }

  TEST(core, parser_silent_ascii) {
    auto const s =
        "-0110 12 -989 f7ad0 11000 527 169 fffffbca10 -1.01e-2 x "
        "123xxx:ppp_~\\/&^123 __10x0x0x droW \":/path to/file\" "
        "https://url.to/some/page.htm";

    auto p = parser::wrap(s);

    EXPECT_TRUE(p.parse(" %~b %~o %~d %~x %~B %~O %~D %~X %~f %~c "
                        "%~s %~a %~A \"%~p\" %~u "));
  }

  TEST(core, parser_silent_utf8) {
    auto const s =
        u8"-0110 12 -989 f7ad0 11000 527 169 fffffbca10 -1.01e-2 x "
        u8"123xxx:ppp_~\\/&^123 __10x0x0x droW \":/path to/file\" "
        u8"https://url.to/some/page.htm";

    auto p = parser::wrap(s);

    EXPECT_TRUE(p.parse(" %~b %~o %~d %~x %~B %~O %~D %~X %~f %~c "
                        "%~s %~a %~A \"%~p\" %~u "));
  }

  TEST(core, parser_single) {
    auto const s =
        "-0100 -17 -99 -f7 11 57 79 7ff 2.0e2 a abc:_~\\/&^123 "
        "_1abc Word \":/path/to/file\" https://url.to/some/page.htm";

    auto p = parser::wrap(s);

    auto i_bin = int64_t {};
    auto i_oct = int64_t {};
    auto i_dec = int64_t {};
    auto i_hex = int64_t {};
    auto u_bin = uint64_t {};
    auto u_oct = uint64_t {};
    auto u_dec = uint64_t {};
    auto u_hex = uint64_t {};
    auto f     = double {};
    auto c     = char32_t {};

    auto str  = u8string {};
    auto id   = u8string {};
    auto word = u8string {};
    auto path = u8string {};
    auto url  = u8string {};

    EXPECT_TRUE(p.parse(
        " %b %o %d %x %B %O %D %X %f %c %s %a %A \"%p\" %u ", //
        &i_bin, &i_oct, &i_dec, &i_hex, &u_bin, &u_oct, &u_dec,
        &u_hex, &f, &c, &str, &id, &word, &path, &url));

    EXPECT_EQ(i_bin, -4);
    EXPECT_EQ(i_oct, -017);
    EXPECT_EQ(i_dec, -99);
    EXPECT_EQ(i_hex, -0xf7);
    EXPECT_EQ(u_bin, 3u);
    EXPECT_EQ(u_oct, 057u);
    EXPECT_EQ(u_dec, 79u);
    EXPECT_EQ(u_hex, 0x7ffu);
    EXPECT_EQ(f, 2.0e2);
    EXPECT_EQ(c, U'a');
    EXPECT_EQ(str, u8string(u8"abc:_~\\/&^123"));
    EXPECT_EQ(id, u8string(u8"_1abc"));
    EXPECT_EQ(word, u8string(u8"Word"));
    EXPECT_EQ(path, u8string(u8":/path/to/file"));
    EXPECT_EQ(url, u8string(u8"https://url.to/some/page.htm"));
  }

  TEST(core, parser_multiple) {
    auto const s = "1 2 3 4 5";
    auto       p = parser::wrap(s);

    EXPECT_TRUE(p.parse(" %~d "));
    EXPECT_TRUE(p.parse(" %~d "));
    EXPECT_TRUE(p.parse(" %~d "));
    EXPECT_TRUE(p.parse(" %~d "));
    EXPECT_TRUE(p.parse(" %~d "));
    EXPECT_FALSE(p.parse(" %~d "));
  }

  TEST(core, parser_push_pop_1) {
    auto const s = "10, 20, 30";
    auto       p = parser::wrap(s);

    auto x = int64_t {};
    auto y = int64_t {};

    EXPECT_TRUE(p.parse(" %d ", &x));
    p.push_offset();
    EXPECT_EQ(x, 10);
    EXPECT_TRUE(p.parse(" , %d ", &x));
    EXPECT_EQ(x, 20);
    p.pop_offset(true);
    EXPECT_TRUE(p.parse(" , %d  , %d ", &x, &y));
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 30);
  }

  TEST(core, parser_push_pop_2) {
    auto const s = "10, 20, 30";
    auto       p = parser::wrap(s);

    auto x = int64_t {};
    auto y = int64_t {};

    EXPECT_TRUE(p.parse(" %d ", &x));
    p.push_offset();
    EXPECT_EQ(x, 10);
    EXPECT_TRUE(p.parse(" , %d ", &x));
    EXPECT_EQ(x, 20);
    p.pop_offset(false);
    EXPECT_TRUE(p.parse(" , %d ", &y));
    EXPECT_EQ(y, 30);
  }

  TEST(core, parser_float) {
    auto const s = "3.1415 0.00031415 31415.001";
    auto       p = parser::wrap(s);

    auto a = double {};
    auto b = double {};
    auto c = double {};

    EXPECT_TRUE(p.parse(" %f %f %f ", &a, &b, &c));
    EXPECT_DOUBLE_EQ(a, 3.1415);
    EXPECT_DOUBLE_EQ(b, 0.00031415);
    EXPECT_DOUBLE_EQ(c, 31415.001);
  }
}
