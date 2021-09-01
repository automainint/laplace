/*  test/unittests/n_transfer.test.cpp
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

#include "../../laplace/network/transfer.h"
#include <gtest/gtest.h>
#include <random>

namespace laplace::test {
  using network::transfer, std::span;

  TEST(network, transfer_pack) {
    constexpr auto test_count = 4;

    auto rdev = std::random_device {};
    auto dist = std::uniform_int_distribution<sl::index>(1, 200);

    for (sl::index n = 0; n < test_count; n++) {
      auto msg = vbyte(dist(rdev));

      for (sl::index i = 0; i < msg.size(); i++) {
        msg[i] = static_cast<uint8_t>(rdev());
      }

      auto msgs = sl::vector<span_cbyte> {};
      msgs.emplace_back(span_cbyte { msg });

      auto alice = transfer {};
      auto bob   = transfer {};

      auto enc = alice.encode(msgs);
      auto dec = bob.decode(enc);

      EXPECT_FALSE(enc.empty());
      EXPECT_EQ(dec.size(), 1u);

      if (dec.size() == 1) {
        EXPECT_TRUE(dec[0] == msg);
      }
    }
  }
}
