/*  test/unittests/nc_dh_rabbit.test.cpp
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

#include "../../laplace/core/serial.h"
#include "../../laplace/network/crypto/ecc_rabbit.h"
#include <gtest/gtest.h>
#include <random>
#include <string>

namespace laplace::test {
  namespace crypto = network::crypto;

  using crypto::ecc_rabbit, std::string, serial::wr;

  TEST(network, ecc_key_generation) {
    constexpr sl::index test_count = 1;

    for (sl::index i = 0; i < test_count; i++) {
      ecc_rabbit alice, bob;

      EXPECT_NE(alice.get_public_key().size(), 0u);
      EXPECT_NE(bob.get_public_key().size(), 0u);

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      EXPECT_NE(alice.get_mutual_key().size(), 0u);
      EXPECT_NE(bob.get_mutual_key().size(), 0u);

      const auto akey = alice.get_mutual_key();
      const auto bkey = bob.get_mutual_key();

      EXPECT_NE(akey.size(), 0u);
      EXPECT_EQ(akey.size(), bkey.size());

      if (!akey.empty() && !bkey.empty() && akey.size() == bkey.size()) {
        EXPECT_EQ(memcmp(akey.data(), bkey.data(), akey.size()), 0);
      }
    }
  }

  TEST(network, rabbit_encryption) {
    constexpr sl::index test_count = 1;

    for (sl::index i = 0; i < test_count; i++) {
      ecc_rabbit alice, bob;

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      vbyte msg0(10);
      vbyte msg1(10);
      vbyte msg2(10);

      vbyte enc0 = alice.encrypt(msg0);
      vbyte enc1 = alice.encrypt(msg1);
      vbyte enc2 = alice.encrypt(msg2);

      EXPECT_FALSE(enc0.empty());
      EXPECT_FALSE(enc1.empty());
      EXPECT_FALSE(enc2.empty());

      EXPECT_FALSE(msg0 == enc0);
      EXPECT_FALSE(msg1 == enc1);
      EXPECT_FALSE(msg2 == enc2);

      EXPECT_TRUE(msg0 == bob.decrypt(enc0));
      EXPECT_TRUE(msg1 == bob.decrypt(enc1));
      EXPECT_TRUE(msg2 == bob.decrypt(enc2));
    }
  }

  TEST(network, rabbit_packet_loss) {
    constexpr sl::index test_count = 1;

    std::random_device rdev;

    for (sl::index i = 0; i < test_count; i++) {
      ecc_rabbit alice, bob;
      alice.set_verbose(true);
      bob.set_verbose(true);

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      vbyte msg0(50);
      vbyte msg1(50);
      vbyte msg2(50);

      vbyte baaad(128);

      for (sl::index k = 0; k < baaad.size(); k += 8)
        wr(baaad, k, static_cast<uint64_t>(rdev()));

      auto enc0 = alice.encrypt(msg0);
      static_cast<void>(alice.encrypt(msg1));
      auto enc2 = alice.encrypt(msg2);

      EXPECT_TRUE(msg0 == bob.decrypt(enc0));
      std::ignore = bob.decrypt(baaad);
      EXPECT_TRUE(msg2 == bob.decrypt(enc2));
    }
  }
}
