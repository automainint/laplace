/*  test/unittests/c_embedded.test.cpp
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

#include "../../laplace/core/embedded.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, embedded_scan) {
    EXPECT_FALSE(embedded::scan(L""));
    EXPECT_FALSE(embedded::scan(L"regular/file/name"));
    EXPECT_TRUE(embedded::scan(L":/embedded/file/name"));
  }

  TEST(core, embedded_exists) {
    EXPECT_FALSE(embedded::exists(L""));
    EXPECT_FALSE(embedded::exists(L":/a?~!&*^$#"));
    EXPECT_TRUE(embedded::exists(L":/shaders/flat/solid.vert"));
    EXPECT_FALSE(embedded::exists(L":/z?~!&*^$#"));
  }

  TEST(core, embedded_open) {
    EXPECT_TRUE(embedded::open(L"").empty());
    EXPECT_TRUE(embedded::open(L":/a?~!&*^$#").empty());
    EXPECT_FALSE(
        embedded::open(L":/shaders/flat/solid.vert").empty());
    EXPECT_TRUE(embedded::open(L":/z?~!&*^$#").empty());
  }
}
