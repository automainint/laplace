/*  test/unittests/f_text.test.cpp
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

#include "../../laplace/format/binary.h"
#include "../../laplace/format/text.h"
#include "../../laplace/format/utils.h"
#include <gtest/gtest.h>
#include <sstream>

namespace laplace::test {
  namespace binary = format::binary;
  namespace text   = format::text;

  using core::family, core::vfamily, binary::encode, binary::decode,
      format::wrap, std::ostringstream, std::istringstream,
      std::string;

  TEST(format, binary_empty) {
    auto src = family {};
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_true) {
    auto src = family { true };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_false) {
    auto src = family { false };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_integer) {
    auto src = family { 100 };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_uint) {
    auto src = family { 0x8000000000000000 };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float) {
    auto src = family { 3.1415 };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float2) {
    auto src = family { 0.0000001 };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_float3) {
    auto src = family { 1e100 };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_string) {
    auto src = family { u8"Foo bar" };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_bytes) {
    auto src = family { vbyte { 1, 2, 3, 4, 5 } };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_bitfield) {
    auto src = family { vfamily { true, false, false, true, true,
                                  true, false, true, false, true } };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_vector) {
    auto src = family { vfamily { 1, 2, 3, 4, 5 } };
    auto ss  = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_composite) {
    auto src   = family {};
    src["foo"] = 42;
    src["bar"] = "Hello";

    auto ss = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }

  TEST(format, binary_function) {
    auto src = family {};

    src[text::s_function]     = "foo";
    src[text::s_arguments][0] = "bar";
    src[text::s_arguments][1] = 42;

    auto ss = ostringstream {};

    encode(wrap(ss), src);

    auto s   = ss.str();
    auto in  = istringstream { s };
    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && src == *dst);
  }
}
