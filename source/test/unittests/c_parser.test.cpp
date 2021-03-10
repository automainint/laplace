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

  TEST(core, parser) {
    const auto s =
        "-0100 -17 -99 -f7 11 57 79 7ff 2.0e2 a abc:_~\\/&^123 "
        "_1abc Word \":/path/to/file\" https://url.to/some/page.htm";

    auto p = parser::wrap(s);

    int64_t  i_bin = 0;
    int64_t  i_oct = 0;
    int64_t  i_dec = 0;
    int64_t  i_hex = 0;
    uint64_t u_bin = 0;
    uint64_t u_oct = 0;
    uint64_t u_dec = 0;
    uint64_t u_hex = 0;
    double   f     = 0;
    char32_t c     = 0;

    u8string str;
    u8string id;
    u8string word;
    u8string path;
    u8string url;

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
