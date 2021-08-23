/*  test/unittests/c_family.test.cpp
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

#include "../../laplace/core/family.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::family, core::composite, core::vfamily, std::u8string,
      std::string_view, std::wstring_view, std::u8string_view,
      std::u16string_view, std::u32string_view;

  TEST(core, family_construct) {
    EXPECT_TRUE(family().is_empty());
    EXPECT_TRUE(family {}.is_empty());
    EXPECT_TRUE(family(vfamily { family() }).is_vector());
    EXPECT_TRUE(
        family(composite { { family(), family() } }).is_composite());
    EXPECT_TRUE(family(true).is_boolean());
    EXPECT_TRUE(family(int8_t(0)).is_integer());
    EXPECT_TRUE(family(int16_t(0)).is_integer());
    EXPECT_TRUE(family(int32_t(0)).is_integer());
    EXPECT_TRUE(family(int64_t(0)).is_integer());
    EXPECT_TRUE(family(uint8_t(0)).is_uint());
    EXPECT_TRUE(family(uint16_t(0)).is_uint());
    EXPECT_TRUE(family(uint32_t(0)).is_uint());
    EXPECT_TRUE(family(uint64_t(0)).is_uint());
    EXPECT_TRUE(family(0.f).is_real());
    EXPECT_TRUE(family(0.).is_real());
    EXPECT_TRUE(family(u8string()).is_string());
    EXPECT_TRUE(family("").is_string());
    EXPECT_TRUE(family(L"").is_string());
    EXPECT_TRUE(family(u8"").is_string());
    EXPECT_TRUE(family(u"").is_string());
    EXPECT_TRUE(family(U"").is_string());
    EXPECT_TRUE(family(string_view("")).is_string());
    EXPECT_TRUE(family(wstring_view(L"")).is_string());
    EXPECT_TRUE(family(u16string_view(u"")).is_string());
    EXPECT_TRUE(family(u32string_view(U"")).is_string());
    EXPECT_TRUE(family(vbyte { 0 }).is_bytes());
  }

  TEST(core, family_assign) {
    family f;

    f = vfamily { family() };
    EXPECT_TRUE(f.is_vector());

    f = composite { { family(), family() } };
    EXPECT_TRUE(f.is_composite());

    f = true;
    EXPECT_TRUE(f.is_boolean());

    f = int8_t(0);
    EXPECT_TRUE(f.is_integer());

    f = int16_t(0);
    EXPECT_TRUE(f.is_integer());

    f = int32_t(0);
    EXPECT_TRUE(f.is_integer());

    f = int64_t(0);
    EXPECT_TRUE(f.is_integer());

    f = uint8_t(0);
    EXPECT_TRUE(f.is_uint());

    f = uint16_t(0);
    EXPECT_TRUE(f.is_uint());

    f = uint32_t(0);
    EXPECT_TRUE(f.is_uint());

    f = uint64_t(0);
    EXPECT_TRUE(f.is_uint());

    f = 0.f;
    EXPECT_TRUE(f.is_real());

    f = 0.;
    EXPECT_TRUE(f.is_real());

    f = u8string();
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

    f = string_view("");
    EXPECT_TRUE(f.is_string());

    f = u8string_view(u8"");
    EXPECT_TRUE(f.is_string());

    f = wstring_view(L"");
    EXPECT_TRUE(f.is_string());

    f = u16string_view(u"");
    EXPECT_TRUE(f.is_string());

    f = u32string_view(U"");
    EXPECT_TRUE(f.is_string());

    f = vbyte { 0 };
    EXPECT_TRUE(f.is_bytes());
  }

  TEST(core, family_compare) {
    EXPECT_TRUE(family(true) == family(true));
    EXPECT_TRUE(family(false) == family(false));
    EXPECT_TRUE(family(false) < family(true));

    EXPECT_TRUE(family(0) == family(0));
    EXPECT_TRUE(family(0) < family(1));

    EXPECT_TRUE(family(0.) == family(0.));
    EXPECT_TRUE(family(0.) < family(1.));

    EXPECT_TRUE(family("abc") == family("abc"));
    EXPECT_TRUE(family("ab") < family("abc"));
    EXPECT_TRUE(family("abc") < family("bc"));

    EXPECT_TRUE(family(0u) == family(0u));
    EXPECT_TRUE(family(0u) < family(1u));

    EXPECT_TRUE(family(vbyte { 0, 1, 2 }) == family(vbyte { 0, 1, 2 }));
    EXPECT_TRUE(family(vbyte { 0, 1 }) < family(vbyte { 0, 1, 2 }));
    EXPECT_TRUE(family(vbyte { 0, 1, 2 }) < family(vbyte { 1, 2 }));
    EXPECT_TRUE(family(vbyte { 0, 1, 2 }) < family(vbyte { 1, 0 }));

    EXPECT_TRUE(family(vfamily { family(0), family(1), family(2) }) ==
                family(vfamily { family(0), family(1), family(2) }));
    EXPECT_TRUE(family(vfamily { family(0), family(1) }) <
                family(vfamily { family(0), family(1), family(2) }));
    EXPECT_TRUE(family(vfamily { family(0), family(1), family(2) }) <
                family(vfamily { family(1), family(2) }));
    EXPECT_TRUE(family(vfamily { family(0), family(1), family(2) }) <
                family(vfamily { family(1), family(0) }));

    EXPECT_TRUE(family(composite { { family(0), family(1) } }) ==
                family(composite { { family(0), family(1) } }));
    EXPECT_TRUE(family(composite { { family(0), family(1) } }) <
                family(composite { { family(0), family(1) },
                                   { family(2), family(3) } }));
    EXPECT_TRUE(family(composite { { family(0), family(1) },
                                   { family(2), family(3) } }) <
                family(composite { { family(2), family(3) } }));
  }

  TEST(core, family_nothrow) {
    family       f;
    const family cf;

    EXPECT_FALSE(f.get_boolean());
    EXPECT_EQ(f.get_integer(), 0ll);
    EXPECT_EQ(f.get_real(), 0.);
    EXPECT_TRUE(f.get_string().empty());
    EXPECT_EQ(f.get_uint(), 0ull);

    EXPECT_EQ(f["abc"], family());
    EXPECT_EQ(f[0], family());
    EXPECT_EQ(f["a"][1]["b"][3], family());

    EXPECT_FALSE(cf.get_boolean());
    EXPECT_EQ(cf.get_integer(), 0ll);
    EXPECT_EQ(cf.get_real(), 0.);
    EXPECT_TRUE(cf.get_string().empty());
    EXPECT_EQ(cf.get_uint(), 0ull);

    EXPECT_EQ(cf["abc"], family());
    EXPECT_EQ(cf[0], family());
    EXPECT_EQ(cf["a"][1]["b"][3], family());
  }

  TEST(core, family_get_string) {
    EXPECT_EQ(family()["unexisting value"].get_string(),
              u8string_view(u8""));
    EXPECT_EQ(family(u8"my string").get_string(),
              u8string_view(u8"my string"));
  }
}
