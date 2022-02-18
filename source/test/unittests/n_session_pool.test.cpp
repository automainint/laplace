/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/session_pool.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::session_pool, std::string_view;

  TEST(network, session_pool_blank) {
    auto pool = session_pool {};
    EXPECT_EQ(pool.get_count(), 0);
    EXPECT_FALSE(pool.is_full());
  }

  TEST(network, session_pool_max_count) {
    auto pool = session_pool {};
    pool.set_max_count(0);
    EXPECT_TRUE(pool.is_full());
  }

  TEST(network, session_pool_add) {
    auto pool = session_pool {};
    pool.add("127.0.0.1", 1);
    EXPECT_EQ(pool.get_count(), 1);
  }

  TEST(network, session_pool_clear) {
    auto pool = session_pool {};
    pool.add("127.0.0.1", 1);
    pool.clear();
    EXPECT_EQ(pool.get_count(), 0);
  }

  TEST(network, session_pool_erase_disconnected) {
    auto pool = session_pool {};
    pool.add("127.0.0.1", 1);
    pool.add("127.0.0.1", 2);
    pool[0].is_connected = false;
    pool[1].is_connected = true;
    pool.erase_disconnected();
    EXPECT_EQ(pool.get_count(), 1);
    EXPECT_EQ(pool[0].port, 2);
  }

  TEST(network, session_pool_has) {
    auto pool = session_pool {};
    pool.add("127.0.0.1", 1);
    EXPECT_TRUE(pool.has(0));
    EXPECT_FALSE(pool.has(1));
    EXPECT_FALSE(pool.has(-1));
  }

  TEST(network, session_pool_find) {
    auto pool = session_pool {};
    pool.add("127.0.0.1", 1);
    EXPECT_EQ(pool.find("127.0.0.1", 1), 0);
    EXPECT_EQ(pool.find("127.0.0.1", 2), network::index_undefined);
    EXPECT_EQ(
        pool.find("127.0.0.1", 2, session_pool::add_if_not_found), 1);
  }

  TEST(network, session_pool_is_full) {
    auto pool = session_pool {};
    pool.set_max_count(1);
    pool.add("127.0.0.1", 1);
    std::ignore = pool.find("127.0.0.1", 2,
                            session_pool::add_if_not_found);
    EXPECT_TRUE(pool.is_full());
    EXPECT_EQ(pool.get_count(), 1);
  }
}
