/*  test/unittests/c_element.test.cpp
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

#include "../../laplace/core/element.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using core::element;

  class my_element : public element {
  public:
    my_element() {
      set_type_id(1);
    }

    ~my_element() override = default;
  };

  TEST(core, element_type_id) {
    element e;
    EXPECT_EQ(e.get_type_id(), element::type_id());
  }

  TEST(core, element_inheritance) {
    my_element e;

    EXPECT_EQ(e.get_type_id(), 1u);
  }
}
