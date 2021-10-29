/*  test/unittests/c_profiler.test.cpp
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

#include "../../laplace/core/profiler.h"
#include <gtest/gtest.h>
#include <thread>

namespace laplace::test {
  namespace pro = core::profiler;

  using std::string_view, std::chrono::microseconds,
      std::this_thread::sleep_for;

  TEST(core, profiler_no_setup) {
    auto foo = string_view { "test foo" };
    auto bar = string_view { "test bar" };

    pro::setup(pro::fn_segment {});

    pro::fence(foo);
    pro::fence(bar);
    pro::fence("");
  }

  TEST(core, profiler_setup_and_fence) {
    auto foo = string_view { "test foo" };
    auto bar = string_view { "test bar" };
    auto ok  = false;

    pro::setup([&](string_view begin, string_view end, sl::time dur) {
      if (begin == foo && end == bar && dur >= 0 && dur < 10000)
        ok = true;
    });

    pro::fence(foo);
    pro::fence(bar);
    pro::fence("");

    pro::setup(pro::fn_segment {});

    EXPECT_TRUE(ok);
  }

  TEST(core, profiler_sleep) {
    auto foo        = string_view { "test foo" };
    auto bar        = string_view { "test bar" };
    auto sleep_time = 10000;
    auto duration   = sl::time { -1 };

    pro::setup([&](string_view begin, string_view end, sl::time dur) {
      if (begin == foo && end == bar && dur >= 0)
        duration = dur;
    });

    pro::fence(foo);
    sleep_for(microseconds(sleep_time));
    pro::fence(bar);
    pro::fence("");

    pro::setup(pro::fn_segment {});

    EXPECT_GE(duration, sleep_time);
  }
}
