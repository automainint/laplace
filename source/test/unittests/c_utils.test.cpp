/*  test/unittests/c_utils.test.cpp
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

#include "../../laplace/core/utils.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, utils_index_arg_types) {
    EXPECT_EQ(as_index(static_cast<signed char>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<signed short>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<signed int>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<signed long>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<signed long long>(0)),
              sl::index {});

    EXPECT_EQ(as_index(static_cast<unsigned char>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<unsigned short>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<unsigned int>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<unsigned long>(0)), sl::index {});
    EXPECT_EQ(as_index(static_cast<unsigned long long>(0)),
              sl::index {});
  }

  TEST(core, utils_index_unsigned) {
    if constexpr (sizeof(sl::index) == 8) {
      constexpr uint64_t a = 0x1020304050607080;
      constexpr uint64_t b = 0x2030405060708090;
      constexpr uint64_t c = 0x8000000000000000;

      EXPECT_EQ(as_index(a), 0x1020304050607080ull);
      EXPECT_EQ(as_index(b), 0x2030405060708090ull);
      EXPECT_EQ(as_index(c, -1, true), -1);
    }

    if constexpr (sizeof(sl::index) == 4) {
      constexpr uint64_t a = 0x10203040;
      constexpr uint64_t b = 0x20304050;
      constexpr uint64_t c = 0x80000000;

      EXPECT_EQ(as_index(a), 0x10203040u);
      EXPECT_EQ(as_index(b), 0x20304050u);
      EXPECT_EQ(as_index(c, -1, true), -1);
    }
  }

  TEST(core, utils_index_signed) {
    EXPECT_EQ(as_index(int64_t { -10 }, -1, true), -1);

    if constexpr (sizeof(sl::index) == 4) {
      EXPECT_EQ(as_index(int64_t { 0x1000000000 }, -1, true), -1);
    }
  }
}
