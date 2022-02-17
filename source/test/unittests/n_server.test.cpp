/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/serial.h"
#include "../../laplace/network/crypto/ecc_rabbit.h"
#include "../../laplace/network/pipe.h"
#include "../../laplace/network/server.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  /*  If encryption is enabled, all the requests and responses should
   *  be encrypted except for session_request and session_response.
   */

  TEST(network, server_initial_state) {
    auto alice = server {};
    EXPECT_FALSE(alice.has_token());
    EXPECT_FALSE(alice.is_connected());
    EXPECT_FALSE(alice.is_quit());
  }
}
