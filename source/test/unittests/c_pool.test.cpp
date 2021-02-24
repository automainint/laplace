/*  test/unittests/c_pool.test.cpp
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

#include "../../laplace/core/pool.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using std::make_shared, core::pool, core::element, std::wstring;

  TEST(core, pool_contains) {
    pool p;

    wstring a = L"my file";
    wstring b = L"file2";

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));
  }

  TEST(core, pool_store) {
    pool p;

    wstring name = L"my file";

    EXPECT_FALSE(p.contains(name));

    p.store(name, make_shared<element>());

    EXPECT_TRUE(p.contains(name));
  }

  TEST(core, pool_release) {
    pool p;

    wstring name = L"file2";

    EXPECT_FALSE(p.contains(name));

    p.store(name, make_shared<element>());

    EXPECT_TRUE(p.contains(name));
  }

  TEST(core, pool_clear) {
    pool p;

    wstring a = L"file1";
    wstring b = L"file2";

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));

    p.store(a, make_shared<element>());
    p.store(b, make_shared<element>());

    EXPECT_TRUE(p.contains(a));
    EXPECT_TRUE(p.contains(b));

    p.clear();

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));
  }

  TEST(core, pool_load) {
    pool p;

    wstring a = L"file1";
    wstring b = L"file2";

    EXPECT_TRUE(!p.load(a));
    EXPECT_TRUE(!p.load(b));

    auto obj1 = make_shared<element>();
    auto obj2 = make_shared<element>();

    p.store(a, obj1);
    p.store(b, obj2);

    EXPECT_EQ(p.load(a), obj1);
    EXPECT_EQ(p.load(b), obj2);
  }
}
