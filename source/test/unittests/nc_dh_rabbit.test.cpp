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

#include "../../laplace/core/utils.h"
#include "../../laplace/network/crypto/dh_rabbit.h"
#include "../../laplace/network/crypto/prime.h"
#include <gtest/gtest.h>
#include <string>

namespace laplace::test {
  namespace crypto = network::crypto;

  using crypto::dh_rabbit, crypto::prime, crypto::generator,
      std::string;

  TEST(network, wolfssl_dh) {
    constexpr size_t test_count = 4;
    constexpr size_t key_size   = 256;

    for (size_t i = 0; i < test_count; i++) {
      WC_RNG alice_random;
      DhKey  alice_key;

      uint32_t alice_public_size  = key_size;
      uint32_t alice_private_size = key_size;
      uint32_t alice_mutual_size  = 0;

      uint8_t alice_public[key_size]  = { 0 };
      uint8_t alice_private[key_size] = { 0 };
      uint8_t alice_mutual[key_size]  = { 0 };

      WC_RNG bob_random;
      DhKey  bob_key;

      uint32_t bob_public_size  = key_size;
      uint32_t bob_private_size = key_size;
      uint32_t bob_mutual_size  = 0;

      uint8_t bob_public[key_size]  = { 0 };
      uint8_t bob_private[key_size] = { 0 };
      uint8_t bob_mutual[key_size]  = { 0 };

      wc_InitRng(&alice_random);
      wc_InitDhKey(&alice_key);

      wc_DhSetKey( //
          &alice_key, prime, sizeof prime, generator, sizeof generator);

      wc_DhGenerateKeyPair( //
          &alice_key, &alice_random, alice_private,
          &alice_private_size, alice_public, &alice_public_size);

      wc_InitRng(&bob_random);
      wc_InitDhKey(&bob_key);

      wc_DhSetKey( //
          &bob_key, prime, sizeof prime, generator, sizeof generator);

      wc_DhGenerateKeyPair( //
          &bob_key, &bob_random, bob_private, &bob_private_size,
          bob_public, &bob_public_size);

      wc_DhAgree( //
          &alice_key, alice_mutual, &alice_mutual_size, alice_private,
          alice_private_size, bob_public, bob_public_size);

      wc_DhAgree( //
          &bob_key, bob_mutual, &bob_mutual_size, bob_private,
          bob_private_size, alice_public, alice_public_size);

      EXPECT_EQ(alice_mutual_size, bob_mutual_size);
      EXPECT_EQ(memcmp(alice_mutual, bob_mutual, alice_mutual_size), 0);
    }
  }

  TEST(network, wolfssl_rabbit) {
    constexpr size_t test_count = 4;

    for (size_t i = 0; i < test_count; i++) {
      Rabbit alice = { 0 };
      Rabbit bob   = { 0 };

      uint8_t key[16] = { 0 };
      for (size_t i = 0; i < 16; i++) {
        key[i] = static_cast<uint8_t>(rand());
      }

      const uint8_t test[100] = { 0 };
      uint8_t       enc[100]  = { 0 };
      uint8_t       dec[100]  = { 0 };

      wc_RabbitSetKey(&alice, key, nullptr);
      wc_RabbitSetKey(&bob, key, nullptr);

      wc_RabbitProcess(&alice, enc, test, 100);
      wc_RabbitProcess(&bob, dec, enc, 100);

      EXPECT_EQ(memcmp(test, dec, 100), 0);
    }
  }

  TEST(network, dh_key_generation) {
    constexpr size_t test_count = 4;

    for (size_t i = 0; i < test_count; i++) {
      dh_rabbit alice, bob;

      EXPECT_NE(alice.get_public_key().size(), 0);
      EXPECT_NE(bob.get_public_key().size(), 0);

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      EXPECT_NE(alice.get_mutual_key().size(), 0);
      EXPECT_NE(bob.get_mutual_key().size(), 0);

      const auto akey = alice.get_mutual_key();
      const auto bkey = bob.get_mutual_key();

      EXPECT_EQ(akey.size(), bkey.size());
      EXPECT_EQ(memcmp(akey.data(), bkey.data(), akey.size()), 0);
    }
  }

  TEST(network, rabbit_encryption) {
    constexpr size_t test_count = 4;

    for (size_t i = 0; i < test_count; i++) {
      dh_rabbit alice, bob;

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      vbyte msg0(10);
      vbyte msg1(10);
      vbyte msg2(10);

      vbyte enc0 = alice.encrypt(msg0);
      vbyte enc1 = alice.encrypt(msg1);
      vbyte enc2 = alice.encrypt(msg2);

      EXPECT_FALSE(msg0 == enc0);
      EXPECT_FALSE(msg1 == enc1);
      EXPECT_FALSE(msg2 == enc2);

      EXPECT_TRUE(msg0 == bob.decrypt(enc0));
      EXPECT_TRUE(msg1 == bob.decrypt(enc1));
      EXPECT_TRUE(msg2 == bob.decrypt(enc2));
    }
  }

  TEST(network, rabbit_packet_loss) {
    constexpr size_t test_count = 1;

    for (size_t i = 0; i < test_count; i++) {
      dh_rabbit alice, bob;

      alice.set_remote_key(bob.get_public_key());
      bob.set_remote_key(alice.get_public_key());

      vbyte msg0(50);
      vbyte msg1(50);
      vbyte msg2(50);

      vbyte baaad(128);
      wr<uint64_t>(baaad, 0, 7000);

      auto enc0 = alice.encrypt(msg0);
      static_cast<void>(alice.encrypt(msg1));
      auto enc2 = alice.encrypt(msg2);

      EXPECT_TRUE(msg0 == bob.decrypt(enc0));
      static_cast<void>(bob.decrypt(baaad));
      EXPECT_TRUE(msg2 == bob.decrypt(enc2));
    }
  }
}
