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
    EXPECT_TRUE(unival(int8_t(0)).is_integer());
    EXPECT_TRUE(unival(int16_t(0)).is_integer());
    EXPECT_TRUE(unival(int32_t(0)).is_integer());
    EXPECT_TRUE(unival(int64_t(0)).is_integer());
    EXPECT_TRUE(unival(uint8_t(0)).is_uint());
    EXPECT_TRUE(unival(uint16_t(0)).is_uint());
    EXPECT_TRUE(unival(uint32_t(0)).is_uint());
    EXPECT_TRUE(unival(uint64_t(0)).is_uint());
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

    f = int8_t {};
    EXPECT_TRUE(f.is_integer());

    f = int16_t {};
    EXPECT_TRUE(f.is_integer());

    f = int32_t {};
    EXPECT_TRUE(f.is_integer());

    f = int64_t {};
    EXPECT_TRUE(f.is_integer());

    f = uint8_t {};
    EXPECT_TRUE(f.is_uint());

    f = uint16_t {};
    EXPECT_TRUE(f.is_uint());

    f = uint32_t {};
    EXPECT_TRUE(f.is_uint());

    f = uint64_t {};
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
    EXPECT_TRUE(unival(0) < unival(1));

    EXPECT_TRUE(unival(0.) == unival(0.));
    EXPECT_TRUE(unival(0.) < unival(1.));

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
    EXPECT_TRUE(unival(unival::composite { { 0, 1 } }) <
                unival(unival::composite { { 0, 1 }, { 2, 3 } }));
    EXPECT_TRUE(unival(unival::composite { { 0, 1 }, { 2, 3 } }) <
                unival(unival::composite { { 2, 3 } }));
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
    EXPECT_EQ(unival()["unexisting value"].get_string(),
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
}
