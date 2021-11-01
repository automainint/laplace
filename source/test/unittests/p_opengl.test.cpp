/*  test/unittests/p_opengl.test.cpp
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

#include "../../laplace/platform/opengl.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(platform, opengl_is_ok) {
    EXPECT_FALSE(gl::is_ok());
  }

  TEST(platform, opengl_require_extensions) {
    gl::require_extensions({});
  }

  TEST(platform, opengl_has_extension) {
    EXPECT_FALSE(gl::has_extension(""));
  }

  TEST(platform, opengl_load_functions) {
    EXPECT_FALSE(gl::load_functions());
  }
}
