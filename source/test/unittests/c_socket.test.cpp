/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/socket.h"
#include <gtest/gtest.h>

namespace laplace::test {
  TEST(core, socket_library) {
    auto sockets = socket_library {};
  }

  TEST(core, socket_error_codes) {
    std::ignore = socket_inprogress();
    std::ignore = socket_wouldblock();
    std::ignore = socket_msgsize();
    std::ignore = socket_isconn();
    std::ignore = socket_connreset();
    std::ignore = socket_addrinuse();
  }

  TEST(core, socket_error) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_EQ(socket_error(), 0);
    EXPECT_EQ(socket_close(s), 0);
  }

  TEST(core, socket_tcp) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_NE(s, -1);
    EXPECT_EQ(socket_close(s), 0);
  }

  TEST(core, socket_udp) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    EXPECT_NE(s, -1);
    EXPECT_EQ(socket_close(s), 0);
  }

  TEST(core, socket_v6_tcp) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_NE(s, -1);
    EXPECT_EQ(socket_close(s), 0);
  }

  TEST(core, socket_v6_udp) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    EXPECT_NE(s, -1);
    EXPECT_EQ(socket_close(s), 0);
  }

  TEST(core, socket_set_blocking) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_EQ(socket_set_blocking(s), 0);
    socket_close(s);
  }

  TEST(core, socket_set_nonblocking) {
    auto sockets = socket_library {};
    auto s       = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_EQ(socket_set_nonblocking(s), 0);
    socket_close(s);
  }
}
