/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/format/binary.h"
#include "../../laplace/format/text.h"
#include "../../laplace/format/utils.h"
#include <gtest/gtest.h>
#include <sstream>

namespace laplace::test {
  namespace binary = format::binary;
  namespace text   = format::text;

  using core::unival, binary::encode, binary::decode, format::wrap,
      std::ostringstream, std::istringstream, std::string;

  TEST(format, binary_empty) {
    auto src = unival {};
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_true) {
    auto src = unival { true };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_false) {
    auto src = unival { false };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_integer) {
    auto src = unival { 100 };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_uint) {
    auto src = unival { 0x8000000000000000 };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float) {
    auto src = unival { 3.1415 };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float2) {
    auto src = unival { 0.0000001 };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float3) {
    auto src = unival { 1e100 };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_string) {
    auto src = unival { u8"Foo bar" };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_bytes) {
    auto src = unival { vbyte { 1, 2, 3, 4, 5 } };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_bitfield) {
    auto src = unival { sl::vector<unival> { true, false, false, true,
                                             true, true, false, true,
                                             false, true } };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_vector) {
    auto src = unival { sl::vector<unival> { 1, 2, 3, 4, 5 } };
    auto ss  = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_composite) {
    auto src   = unival {};
    src["foo"] = 42;
    src["bar"] = "Hello";

    auto ss = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_compact_composite) {
    auto src       = unival {};
    src.by_key(3)  = "Foo";
    src.by_key(1)  = "Bar";
    src.by_key(10) = 42;

    auto ss = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_function) {
    auto src = unival {};

    src[text::s_function]     = "foo";
    src[text::s_arguments][0] = "bar";
    src[text::s_arguments][1] = 42;

    auto ss = ostringstream {};

    auto success = encode(wrap(ss), src, get_global_log());

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(success);
    EXPECT_TRUE(dst && src == *dst);
  }
}
