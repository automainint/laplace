/*  test/unittests/c_parser.test.cpp
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

#include "../../laplace/core/parser.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::parser, std::u8string;

  TEST(core, parser_silent) {
    const auto s =
        "-0110 12 -989 f7ad0 11000 527 169 fffffbca10 -1.01e-2 x "
        "123xxx:ppp_~\\/&^123 __10x0x0x droW \":/path to/file\" "
        "https://url.to/some/page.htm";

    auto p = parser::wrap(s);

    EXPECT_TRUE(p.parse(" %~b %~o %~d %~x %~B %~O %~D %~X %~f %~c "
                        "%~s %~a %~A \"%~p\" %~u "));
  }

  TEST(core, parser) {
    const auto s =
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
}
