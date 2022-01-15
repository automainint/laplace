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
#include "../../laplace/format/text.h"
#include "../../laplace/format/utils.h"
#include <gtest/gtest.h>
#include <sstream>

namespace laplace::test {
  namespace text = format::text;

  using core::unival, core::parser, text::decode, text::encode,
      format::wrap, std::string, std::u8string, std::ostringstream,
      std::istringstream;

  TEST(format, text_encode_empty) {
    auto data = unival {};
    auto s    = ostringstream {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" { } "));
  }

  TEST(format, text_encode_true) {
    auto data = unival { true };
    auto s    = ostringstream {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" true "));
  }

  TEST(format, text_encode_false) {
    auto data = unival { false };
    auto s    = ostringstream {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" false "));
  }

  TEST(format, text_encode_integer) {
    auto data = unival { 1 };
    auto s    = ostringstream {};
    auto x    = int64_t {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" %d ", &x));
    EXPECT_EQ(x, 1);
  }

  TEST(format, text_encode_uint) {
    auto data = unival { 0x8000000000000000ull };
    auto s    = ostringstream {};
    auto x    = uint64_t {};

    auto success = encode(wrap(s), data);

    auto str = s.str();
    auto p   = parser::wrap(str);

    EXPECT_TRUE(success);
    EXPECT_TRUE(p.parse(" 0x") || p.parse(" 0X"));
    EXPECT_TRUE(p.parse("%X ", &x));
    EXPECT_EQ(x, 0x8000000000000000ull);
  }

  TEST(format, text_encode_real) {
    auto data = unival { 1.5 };
    auto s    = ostringstream {};
    auto x    = double {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" %f ", &x));
    EXPECT_DOUBLE_EQ(x, 1.5);
  }

  TEST(format, text_encode_id) {
    auto data = unival { "Hello" };
    auto s    = ostringstream {};
    auto x    = u8string {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" Hello "));
  }

  TEST(format, text_encode_string) {
    auto data = unival { " Hello " };
    auto s    = ostringstream {};
    auto x    = u8string {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" \"% Hello% \" "));
  }

  TEST(format, text_encode_string2) {
    auto data = unival { "\"Hello\"" };
    auto s    = ostringstream {};
    auto x    = u8string {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" \"\\\"Hello\\\"\" "));
  }

  TEST(format, text_encode_string3) {
    auto data = unival { "_abcDEF123" };
    auto s    = ostringstream {};
    auto x    = u8string {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" _abcDEF123 "));
  }

  TEST(format, text_encode_string4) {
    auto data = unival { "1abc" };
    auto s    = ostringstream {};
    auto x    = u8string {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" \"1abc\" "));
  }

  TEST(format, text_encode_bytes) {
    auto data = unival { vbyte { 0x11, 0x7f, 0xa0 } };
    auto s    = ostringstream {};

    auto success = encode(wrap(s), data);

    auto str = s.str();
    auto p   = parser::wrap(str);

    EXPECT_TRUE(success);
    EXPECT_TRUE(p.parse(" :: { 11 7f a0 } ") ||
                p.parse(" :: { 11 7F A0 } "));
  }

  TEST(format, text_encode_vector) {
    auto data = unival { sl::vector<unival> { 1, 2, 3 } };
    auto s    = ostringstream {};

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" 1 , 2 , 3 "));
  }

  TEST(format, text_encode_vector_complex) {
    auto data = unival {};
    auto s    = ostringstream {};

    data[0] = sl::vector<unival> { unival { 1 }, unival { 2 } };
    data[1] = 3;

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" ( 1 , 2 ) , 3 "));
  }

  TEST(format, text_encode_composite) {
    auto data = unival {};
    auto s    = ostringstream {};

    data["a"] = 1;
    data["b"] = 2;

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" { a = 1 b = 2 } "));
  }

  TEST(format, text_encode_function) {
    auto data = unival {};
    auto s    = ostringstream {};

    data[text::s_function]     = "call";
    data[text::s_arguments][0] = 1;
    data[text::s_arguments][1] = 2;

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(" call ( 1 , 2 ) "));
  }

  TEST(format, text_encode_composite_commands) {
    auto data = unival {};
    auto s    = ostringstream {};

    data[text::s_commands][0][text::s_function]     = "call1";
    data[text::s_commands][0][text::s_arguments][0] = 1;
    data[text::s_commands][0][text::s_arguments][1] = 2;
    data[text::s_commands][1][text::s_function]     = "call2";
    data[text::s_commands][1][text::s_arguments][0] = 3;
    data[text::s_commands][1][text::s_arguments][1] = 4;

    auto success = encode(wrap(s), data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(parser::wrap(s.str()).parse(
        " { call1 ( 1 , 2 ) call2 ( 3 , 4 ) } "));
  }

  TEST(format, text_decode_empty) {
    auto s    = istringstream { string { "{}" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_empty());
  }

  TEST(format, text_decode_true) {
    auto s    = istringstream { string { "true" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_boolean() && data->get_boolean());
  }

  TEST(format, text_decode_false) {
    auto s    = istringstream { string { "false" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_boolean() && !data->get_boolean());
  }

  TEST(format, text_decode_integer) {
    auto s    = istringstream { string { "100" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_integer() &&
                data->get_integer() == 100);
  }

  TEST(format, text_decode_uint_hex) {
    auto s    = istringstream { string { "0x8000000000000000" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_uint() &&
                data->get_uint() == 0x8000000000000000ull);
  }

  TEST(format, text_decode_int_hex) {
    auto s    = istringstream { string { "-0x2000000000000000" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_integer() &&
                data->get_integer() == -0x2000000000000000ll);
  }

  TEST(format, text_decode_int_oct) {
    auto s    = istringstream { string { "0777" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_integer() &&
                data->get_integer() == 0777);
  }

  TEST(format, text_decode_int_bin) {
    auto s    = istringstream { string { "0b11110011" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_integer() &&
                data->get_integer() == 0xf3);
  }

  TEST(format, text_decode_float) {
    auto s    = istringstream { string { "3.1415" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_real());

    if (data && data->is_real()) {
      EXPECT_DOUBLE_EQ(data->get_real(), 3.1415);
    }
  }

  TEST(format, text_decode_string) {
    auto s    = istringstream { string { "_Name" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_string() &&
                data->get_string() == u8"_Name");
  }

  TEST(format, text_decode_string2) {
    auto s    = istringstream { string { "\" Name \"" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_string() &&
                data->get_string() == u8" Name ");
  }

  TEST(format, text_decode_string3) {
    auto s    = istringstream { string { "\"\\\"Name\\\"\"" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_string() &&
                data->get_string() == u8"\"Name\"");
  }

  TEST(format, text_decode_vector) {
    auto s    = istringstream { string { "1, 2, 3" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_vector() &&
                data->get_value(0).get_integer() == 1 &&
                data->get_value(1).get_integer() == 2 &&
                data->get_value(2).get_integer() == 3);
  }

  TEST(format, text_decode_vector2) {
    auto s    = istringstream { string { "( 1, 2, 3 )" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_vector() &&
                data->get_value(0).get_integer() == 1 &&
                data->get_value(1).get_integer() == 2 &&
                data->get_value(2).get_integer() == 3);
  }

  TEST(format, text_decode_vector3) {
    auto s    = istringstream { string { "( 1, 2 ), 3" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_vector() &&
                data->get_value(0).get_value(0).get_integer() == 1 &&
                data->get_value(0).get_value(1).get_integer() == 2 &&
                data->get_value(1).get_integer() == 3);
  }

  TEST(format, text_decode_composite) {
    auto s    = istringstream { string { "{ a = 1; b = 2; }" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_composite() &&
                data->get_value("a").get_integer() == 1 &&
                data->get_value("b").get_integer() == 2);
  }

  TEST(format, text_decode_composite2) {
    auto s    = istringstream { string { "{ a: 1; b: 2; }" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_composite() &&
                data->get_value("a").get_integer() == 1 &&
                data->get_value("b").get_integer() == 2);
  }

  TEST(format, text_decode_composite3) {
    auto s    = istringstream { string { "{ a: 1 b = 2 }" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_composite() &&
                data->get_value("a").get_integer() == 1 &&
                data->get_value("b").get_integer() == 2);
  }

  TEST(format, text_decode_bytes) {
    auto s    = istringstream { string { ":: { af b7 cd }" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(
        data && data->is_bytes() && data->get_bytes().size() == 3 &&
        data->get_bytes()[0] == 0xaf &&
        data->get_bytes()[1] == 0xb7 && data->get_bytes()[2] == 0xcd);
  }

  TEST(format, text_decode_function) {
    auto s    = istringstream { string { "call(1, 2)" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(
        data && data->is_composite() &&
        data->get_value(text::s_function).get_string() == u8"call" &&
        data->get_value(text::s_arguments)[0].get_integer() == 1 &&
        data->get_value(text::s_arguments)[1].get_integer() == 2);
  }

  TEST(format, text_decode_composite_commands) {
    auto s    = istringstream { string {
        "{ call1(1, 2); call2(3, 4); }" } };
    auto data = decode(wrap(s));

    EXPECT_TRUE(data && data->is_composite());

    if (data && data->is_composite()) {
      auto &cmds = data->get_value(text::s_commands);

      EXPECT_TRUE(
          cmds[0][text::s_function].get_string() == u8"call1" &&
          cmds[0][text::s_arguments][0].get_integer() == 1 &&
          cmds[0][text::s_arguments][1].get_integer() == 2 &&
          cmds[1][text::s_function].get_string() == u8"call2" &&
          cmds[1][text::s_arguments][0].get_integer() == 3 &&
          cmds[1][text::s_arguments][1].get_integer() == 4);
    }
  }

  TEST(format, text_encode_decode) {
    auto src = unival {};

    src["int"]            = 1;
    src["float"]          = 3.1415;
    src["str"]            = "abc";
    src["vec"][0]         = 1;
    src["vec"][1]         = 2;
    src["vec"][2]         = 3;
    src["values"]["a"]    = "Foo";
    src["values"]["b"]    = "Bar";
    src["values"]["v"][0] = 100;
    src["values"]["v"][1] = 101;
    src["values"]["v"][2] = 102;
    src["values"]["v"][3] = 103;

    auto ss = ostringstream {};

    EXPECT_TRUE(encode(wrap(ss), src));

    auto s  = string { ss.str() };
    auto in = istringstream { s };

    auto dst = decode(wrap(in));

    EXPECT_TRUE(dst && *dst == src);
  }
}
