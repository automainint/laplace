/*  test/unittests/c_unival.test.cpp
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

#include "../../laplace/core/unival.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::unival, std::u8string, std::string_view,
      std::wstring_view, std::u8string_view, std::u16string_view,
      std::u32string_view;

  TEST(core, unival_construct) {
    EXPECT_TRUE(unival().is_empty());
    EXPECT_TRUE(unival {}.is_empty());
    EXPECT_TRUE(unival(sl::vector<unival> { unival {} }).is_vector());
    EXPECT_TRUE(unival(unival::composite { { unival {}, unival {} } })
                    .is_composite());
    EXPECT_TRUE(unival(true).is_boolean());
    EXPECT_TRUE(unival(static_cast<signed char>(0)).is_integer());
    EXPECT_TRUE(unival(static_cast<signed short>(0)).is_integer());
    EXPECT_TRUE(unival(static_cast<signed int>(0)).is_integer());
    EXPECT_TRUE(unival(static_cast<signed long>(0)).is_integer());
    EXPECT_TRUE(
        unival(static_cast<signed long long>(0)).is_integer());
    EXPECT_TRUE(unival(static_cast<unsigned char>(0)).is_uint());
    EXPECT_TRUE(unival(static_cast<unsigned short>(0)).is_uint());
    EXPECT_TRUE(unival(static_cast<unsigned int>(0)).is_uint());
    EXPECT_TRUE(unival(static_cast<unsigned long>(0)).is_uint());
    EXPECT_TRUE(unival(static_cast<unsigned long long>(0)).is_uint());
    EXPECT_TRUE(unival(0.f).is_real());
    EXPECT_TRUE(unival(0.).is_real());
    EXPECT_TRUE(unival(u8string()).is_string());
    EXPECT_TRUE(unival("").is_string());
    EXPECT_TRUE(unival(L"").is_string());
    EXPECT_TRUE(unival(u8"").is_string());
    EXPECT_TRUE(unival(u"").is_string());
    EXPECT_TRUE(unival(U"").is_string());
    EXPECT_TRUE(unival(string_view("")).is_string());
    EXPECT_TRUE(unival(wstring_view(L"")).is_string());
    EXPECT_TRUE(unival(u16string_view(u"")).is_string());
    EXPECT_TRUE(unival(u32string_view(U"")).is_string());
    EXPECT_TRUE(unival(vbyte { 0 }).is_bytes());
  }

  TEST(core, unival_assign) {
    auto f = unival {};

    f = sl::vector<unival> { unival {} };
    EXPECT_TRUE(f.is_vector());

    f = unival::composite { { unival {}, unival {} } };
    EXPECT_TRUE(f.is_composite());

    f = true;
    EXPECT_TRUE(f.is_boolean());

    f = static_cast<signed char>(0);
    EXPECT_TRUE(f.is_integer());

    f = static_cast<signed short>(0);
    EXPECT_TRUE(f.is_integer());

    f = static_cast<signed int>(0);
    EXPECT_TRUE(f.is_integer());

    f = static_cast<signed long>(0);
    EXPECT_TRUE(f.is_integer());

    f = static_cast<signed long long>(0);
    EXPECT_TRUE(f.is_integer());

    f = static_cast<unsigned char>(0);
    EXPECT_TRUE(f.is_uint());

    f = static_cast<unsigned short>(0);
    EXPECT_TRUE(f.is_uint());

    f = static_cast<unsigned int>(0);
    EXPECT_TRUE(f.is_uint());

    f = static_cast<unsigned long>(0);
    EXPECT_TRUE(f.is_uint());

    f = static_cast<unsigned long long>(0);
    EXPECT_TRUE(f.is_uint());

    f = 0.f;
    EXPECT_TRUE(f.is_real());

    f = 0.;
    EXPECT_TRUE(f.is_real());

    f = u8string {};
    EXPECT_TRUE(f.is_string());

    f = "";
    EXPECT_TRUE(f.is_string());

    f = u8"";
    EXPECT_TRUE(f.is_string());

    f = L"";
    EXPECT_TRUE(f.is_string());

    f = u"";
    EXPECT_TRUE(f.is_string());

    f = U"";
    EXPECT_TRUE(f.is_string());

    f = string_view { "" };
    EXPECT_TRUE(f.is_string());

    f = u8string_view { u8"" };
    EXPECT_TRUE(f.is_string());

    f = wstring_view { L"" };
    EXPECT_TRUE(f.is_string());

    f = u16string_view { u"" };
    EXPECT_TRUE(f.is_string());

    f = u32string_view { U"" };
    EXPECT_TRUE(f.is_string());

    f = vbyte { 0 };
    EXPECT_TRUE(f.is_bytes());
  }

  TEST(core, unival_compare) {
    EXPECT_TRUE(unival(true) == unival(true));
    EXPECT_TRUE(unival(false) == unival(false));
    EXPECT_TRUE(unival(false) < unival(true));

    EXPECT_TRUE(unival(0) == unival(0));
    EXPECT_TRUE(unival(0) <= unival(0));
    EXPECT_TRUE(unival(0) >= unival(0));
    EXPECT_TRUE(unival(0) < unival(1));
    EXPECT_TRUE(unival(0) <= unival(1));
    EXPECT_TRUE(unival(1) > unival(0));
    EXPECT_TRUE(unival(1) >= unival(0));

    EXPECT_TRUE(unival(0.) == unival(0.));
    EXPECT_TRUE(unival(0.) < unival(1.));
    EXPECT_TRUE(unival(1.) > unival(0.));

    EXPECT_TRUE(unival("abc") == unival("abc"));
    EXPECT_TRUE(unival("ab") < unival("abc"));
    EXPECT_TRUE(unival("abc") < unival("bc"));

    EXPECT_TRUE(unival(0u) == unival(0u));
    EXPECT_TRUE(unival(0u) < unival(1u));

    EXPECT_TRUE(unival(vbyte { 0, 1, 2 }) ==
                unival(vbyte { 0, 1, 2 }));
    EXPECT_TRUE(unival(vbyte { 0, 1 }) < unival(vbyte { 0, 1, 2 }));
    EXPECT_TRUE(unival(vbyte { 0, 1, 2 }) < unival(vbyte { 1, 2 }));
    EXPECT_TRUE(unival(vbyte { 0, 1, 2 }) < unival(vbyte { 1, 0 }));

    EXPECT_TRUE(unival(sl::vector<unival> { 0, 1, 2 }) ==
                unival(sl::vector<unival> { 0, 1, 2 }));
    EXPECT_TRUE(unival(sl::vector<unival> { 0, 1 }) <
                unival(sl::vector<unival> { 0, 1, 2 }));
    EXPECT_TRUE(unival(sl::vector<unival> { 0, 1, 2 }) <
                unival(sl::vector<unival> { 1, 2 }));
    EXPECT_TRUE(unival(sl::vector<unival> { 0, 1, 2 }) <
                unival(sl::vector<unival> { 1, 0 }));

    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) ==
                unival(unival::composite { { 0, 1 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) <=
                unival(unival::composite { { 0, 1 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) >=
                unival(unival::composite { { 0, 1 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) <
                unival(unival::composite { { 0, 1 }, { 2, 3 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) <=
                unival(unival::composite { { 0, 1 }, { 2, 3 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 }, { 2, 3 } }) <
                unival(unival::composite { { 2, 3 } }));
    EXPECT_TRUE(unival(unival::composite { { 2, 3 } }) >=
                unival(unival::composite { { 0, 1 }, { 2, 3 } }));
  }

  TEST(core, unival_conversion) {
    signed char const        i8  = {};
    signed short const       i16 = {};
    signed int const         i32 = {};
    signed long const        i   = {};
    signed long long const   i64 = {};
    unsigned char const      u8  = {};
    unsigned short const     u16 = {};
    unsigned int const       u32 = {};
    unsigned long const      u   = {};
    unsigned long long const u64 = {};
    double const             f   = {};

    EXPECT_EQ(static_cast<decltype(i8)>(unival { i8 }), i8);
    EXPECT_EQ(static_cast<decltype(i16)>(unival { i16 }), i16);
    EXPECT_EQ(static_cast<decltype(i32)>(unival { i32 }), i32);
    EXPECT_EQ(static_cast<decltype(i)>(unival { i }), i);
    EXPECT_EQ(static_cast<decltype(i64)>(unival { i64 }), i64);
    EXPECT_EQ(static_cast<decltype(u8)>(unival { u8 }), u8);
    EXPECT_EQ(static_cast<decltype(u16)>(unival { u16 }), u16);
    EXPECT_EQ(static_cast<decltype(u32)>(unival { u32 }), u32);
    EXPECT_EQ(static_cast<decltype(u)>(unival { u }), u);
    EXPECT_EQ(static_cast<decltype(u64)>(unival { u64 }), u64);
    EXPECT_EQ(static_cast<decltype(f)>(unival { f }), f);
  }

  TEST(core, unival_operator_index) {
    auto       mut    = unival {};
    auto const nonmut = unival {};

    mut[static_cast<signed char>(0)]        = unival {};
    mut[static_cast<signed short>(0)]       = unival {};
    mut[static_cast<signed int>(0)]         = unival {};
    mut[static_cast<signed long>(0)]        = unival {};
    mut[static_cast<signed long long>(0)]   = unival {};
    mut[static_cast<unsigned char>(0)]      = unival {};
    mut[static_cast<unsigned short>(0)]     = unival {};
    mut[static_cast<unsigned int>(0)]       = unival {};
    mut[static_cast<unsigned long>(0)]      = unival {};
    mut[static_cast<unsigned long long>(0)] = unival {};

    std::ignore = nonmut[static_cast<signed char>(0)];
    std::ignore = nonmut[static_cast<signed short>(0)];
    std::ignore = nonmut[static_cast<signed int>(0)];
    std::ignore = nonmut[static_cast<signed long>(0)];
    std::ignore = nonmut[static_cast<signed long long>(0)];
    std::ignore = nonmut[static_cast<unsigned char>(0)];
    std::ignore = nonmut[static_cast<unsigned short>(0)];
    std::ignore = nonmut[static_cast<unsigned int>(0)];
    std::ignore = nonmut[static_cast<unsigned long>(0)];
    std::ignore = nonmut[static_cast<unsigned long long>(0)];
  }

  TEST(core, unival_nothrow) {
    auto       f  = unival {};
    const auto cf = unival {};

    EXPECT_FALSE(f.get_boolean());
    EXPECT_EQ(f.get_integer(), 0ll);
    EXPECT_EQ(f.get_real(), 0.);
    EXPECT_TRUE(f.get_string().empty());
    EXPECT_EQ(f.get_uint(), 0ull);

    EXPECT_EQ(f["abc"], unival());
    EXPECT_EQ(f[0], unival());
    EXPECT_EQ(f["a"][1]["b"][3], unival());

    EXPECT_FALSE(cf.get_boolean());
    EXPECT_EQ(cf.get_integer(), 0ll);
    EXPECT_EQ(cf.get_real(), 0.);
    EXPECT_TRUE(cf.get_string().empty());
    EXPECT_EQ(cf.get_uint(), 0ull);

    EXPECT_EQ(cf["abc"], unival());
    EXPECT_EQ(cf[0], unival());
    EXPECT_EQ(cf["a"][1]["b"][3], unival());
  }

  TEST(core, unival_get_string) {
    EXPECT_EQ(unival()["nonexistent value"].get_string(),
              u8string_view(u8""));
    EXPECT_EQ(unival(u8"my string").get_string(),
              u8string_view(u8"my string"));
  }

  TEST(core, unival_has_key) {
    auto f = unival {};

    EXPECT_FALSE(f.has("abc"));
  }

  TEST(core, unival_key) {
    auto f = unival {};

    f.key("abc");

    EXPECT_TRUE(f.has("abc"));
  }

  TEST(core, unival_set_key) {
    auto f = unival {};

    f.key("abc");
    f.set_key(0, "def");

    EXPECT_FALSE(f.has("abc"));
    EXPECT_TRUE(f.has("def"));
  }

  TEST(core, unival_merge_simple) {
    auto a = unival {};
    auto b = unival {};
    auto c = unival {};

    a["x"] = 1;
    b["y"] = 2;
    c["y"] = 3;

    EXPECT_TRUE(a.merge(b));
    EXPECT_FALSE(b.merge(c));
    EXPECT_EQ(b["y"].get_integer(), 3);
  }

  TEST(core, unival_logic_error) {
    auto const non_composite = unival { 0 };

    EXPECT_EQ(&non_composite.get_key(0), &unival::logic_error());
    EXPECT_EQ(&non_composite.get_value(0), &unival::logic_error());
  }
}
