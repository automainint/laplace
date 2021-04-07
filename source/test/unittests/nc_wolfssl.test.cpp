/*  test/unittests/nc_wolfssl.test.cpp
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

#include "../../laplace/network/crypto/prime.h"
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include <wolfssl/options.h>

#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/rabbit.h>
#include <wolfssl/wolfcrypt/random.h>

namespace laplace::test {
  namespace crypto = network::crypto;

  using crypto::prime, crypto::generator, std::string, std::array, std::vector;

  TEST(network, wolfssl_dh) {
    constexpr size_t test_count = 1;
    constexpr size_t key_size   = 256;
    constexpr size_t buf_size   = 0x1000;

    for (size_t i = 0; i < test_count; i++) {
      WC_RNG alice_random = {};
      DhKey  alice_key    = {};

      uint32_t alice_public_size  = key_size;
      uint32_t alice_private_size = key_size;
      uint32_t alice_mutual_size  = 0;

      uint8_t alice_public[key_size]  = { 0 };
      uint8_t alice_private[key_size] = { 0 };
      uint8_t alice_mutual[buf_size]  = { 0 };

      WC_RNG bob_random = {};
      DhKey  bob_key    = {};

      uint32_t bob_public_size  = key_size;
      uint32_t bob_private_size = key_size;
      uint32_t bob_mutual_size  = 0;

      uint8_t bob_public[key_size]  = { 0 };
      uint8_t bob_private[key_size] = { 0 };
      uint8_t bob_mutual[buf_size]  = { 0 };

      if (auto _n = wc_InitRng(&alice_random); _n != 0)
        std::cerr << "wc_InitRng failed (code: " << _n << ").\n";

      if (auto _n = wc_InitDhKey(&alice_key); _n != 0)
        std::cerr << "wc_InitDhKey failed (code: " << _n << ").\n";

      if (auto _n = wc_DhSetKey(&alice_key, prime, sizeof prime,
                                generator, sizeof generator);
          _n != 0)
        std::cerr << "wc_DhSetKey failed (code: " << _n << ").\n";

      if (auto _n = wc_DhGenerateKeyPair(
              &alice_key, &alice_random, alice_private,
              &alice_private_size, alice_public, &alice_public_size);
          _n != 0)
        std::cerr << "wc_DhGenerateKeyPair failed (code: " << _n
                  << ").\n";

      if (alice_private_size > key_size) {
        alice_private_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      if (alice_public_size > key_size) {
        alice_public_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      if (auto _n = wc_InitRng(&bob_random); _n != 0)
        std::cerr << "wc_InitRng failed (code: " << _n << ").\n";

      if (auto _n = wc_InitDhKey(&bob_key); _n != 0)
        std::cerr << "wc_InitDhKey failed (code: " << _n << ").\n";

      if (auto _n = wc_DhSetKey(&bob_key, prime, sizeof prime,
                                generator, sizeof generator);
          _n != 0)
        std::cerr << "wc_DhSetKey failed (code: " << _n << ").\n";

      if (auto _n = wc_DhGenerateKeyPair(
              &bob_key, &bob_random, bob_private, &bob_private_size,
              bob_public, &bob_public_size);
          _n != 0)
        std::cerr << "wc_DhGenerateKeyPair failed (code: " << _n
                  << ").\n";

      if (bob_private_size > key_size) {
        bob_private_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      if (bob_public_size > key_size) {
        bob_public_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      if (auto _n = wc_DhAgree(&alice_key, alice_mutual,
                               &alice_mutual_size, alice_private,
                               alice_private_size, bob_public,
                               bob_public_size);
          _n != 0)
        std::cerr << "wc_DhAgree failed (code: " << _n << ").\n";

      if (auto _n = wc_DhAgree(&bob_key, bob_mutual, &bob_mutual_size,
                               bob_private, bob_private_size,
                               alice_public, alice_public_size);
          _n != 0)
        std::cerr << "wc_DhAgree failed (code: " << _n << ").\n";

      if (alice_mutual_size > buf_size) {
        alice_mutual_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      if (bob_mutual_size > buf_size) {
        bob_mutual_size = 0;
        std::cerr << "Buffer overflow.\n";
      }

      EXPECT_NE(alice_mutual_size, 0u);
      EXPECT_EQ(alice_mutual_size, bob_mutual_size);
      EXPECT_EQ(memcmp(alice_mutual, bob_mutual, alice_mutual_size), 0);

      wc_FreeDhKey(&alice_key);
      wc_FreeDhKey(&bob_key);

      wc_FreeRng(&alice_random);
      wc_FreeRng(&bob_random);
    }
  }

  TEST(network, wolfssl_ecc) {
    constexpr size_t test_count = 1;
    constexpr size_t key_size   = 32;
    constexpr size_t buf_size   = 0x1000;

    for (size_t i = 0; i < test_count; i++) {
      WC_RNG  alice_random  = {};
      ecc_key alice_private = {};
      ecc_key alice_public  = {};

      auto     alice_public_buffer = vector<uint8_t>(buf_size);
      uint32_t alice_public_size   = buf_size;
      auto     alice_mutual        = vector<uint8_t>(buf_size);
      uint32_t alice_mutual_size   = buf_size;

      WC_RNG  bob_random  = {};
      ecc_key bob_private = {};
      ecc_key bob_public  = {};

      auto     bob_public_buffer = vector<uint8_t>(buf_size);
      uint32_t bob_public_size   = buf_size;
      auto     bob_mutual        = vector<uint8_t>(buf_size);
      uint32_t bob_mutual_size   = buf_size;

      if (auto _n = wc_InitRng(&alice_random); _n != 0)
        std::cerr << "wc_InitRng failed (code: " << _n << ").\n";

      if (auto _n = wc_InitRng(&bob_random); _n != 0)
        std::cerr << "wc_InitRng failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_init(&alice_private); _n != 0)
        std::cerr << "wc_ecc_init failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_init(&alice_public); _n != 0)
        std::cerr << "wc_ecc_init failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_init(&bob_private); _n != 0)
        std::cerr << "wc_ecc_init failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_init(&bob_public); _n != 0)
        std::cerr << "wc_ecc_init failed (code: " << _n << ").\n";

      #ifdef ECC_TIMING_RESISTANT
      if (auto _n = wc_ecc_set_rng(&alice_private, &alice_random);
          _n != 0)
        std::cerr << "wc_ecc_set_rng failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_set_rng(&bob_private, &bob_random);
          _n != 0)
        std::cerr << "wc_ecc_set_rng failed (code: " << _n << ").\n";
      #endif

      if (auto _n = wc_ecc_make_key(
              &alice_random, key_size, &alice_private);
          _n != 0)
        std::cerr << "wc_ecc_make_key failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_export_x963(&alice_private,
                                       alice_public_buffer.data(),
                                       &alice_public_size);
          _n != 0) {
        alice_public_size = 0;
        std::cerr << "wc_ecc_export_x963 failed (code: " << _n
                  << ").\n";
      }

      if (auto _n = wc_ecc_import_x963(alice_public_buffer.data(),
                                       alice_public_size, &alice_public);
          _n != 0)
        std::cerr << "wc_ecc_import_x963 failed (code: " << _n
                  << ").\n";

      if (auto _n = wc_ecc_make_key(&bob_random, key_size, &bob_private);
          _n != 0)
        std::cerr << "wc_ecc_make_key failed (code: " << _n << ").\n";

      if (auto _n = wc_ecc_export_x963(
              &bob_private, bob_public_buffer.data(), &bob_public_size);
          _n != 0) {
        bob_public_size = 0;
        std::cerr << "wc_ecc_export_x963 failed (code: " << _n
                  << ").\n";
      }

      if (auto _n = wc_ecc_import_x963(
              bob_public_buffer.data(), bob_public_size, &bob_public);
          _n != 0)
        std::cerr << "wc_ecc_import_x963 failed (code: " << _n
                  << ").\n";

      if (auto _n = wc_ecc_shared_secret(&alice_private, &bob_public,
                                         alice_mutual.data(),
                                         &alice_mutual_size);
          _n != 0) {
        alice_mutual_size = 0;
        std::cerr << "wc_ecc_shared_secret failed (code: " << _n
                  << ").\n";
      }

      if (auto _n = wc_ecc_shared_secret(&bob_private, &alice_public,
                                         bob_mutual.data(),
                                         &bob_mutual_size);
          _n != 0) {
        bob_mutual_size = 0;
        std::cerr << "wc_ecc_shared_secret failed (code: " << _n
                  << ").\n";
      }

      EXPECT_NE(alice_mutual_size, 0u);
      EXPECT_EQ(alice_mutual_size, bob_mutual_size);
      EXPECT_TRUE(alice_mutual == bob_mutual);

      wc_ecc_free(&alice_private);
      wc_ecc_free(&alice_public);
      wc_ecc_free(&bob_private);
      wc_ecc_free(&bob_public);

      wc_FreeRng(&alice_random);
      wc_FreeRng(&bob_random);
    }
  }

  TEST(network, wolfssl_rabbit) {
    constexpr size_t test_count = 1;

    std::random_device rdev;

    for (size_t i = 0; i < test_count; i++) {
      Rabbit alice = { 0 };
      Rabbit bob   = { 0 };

      uint8_t key[16] = { 0 };
      for (size_t j = 0; j < 16; j++) {
        key[j] = static_cast<uint8_t>(rdev());
      }

      const uint8_t test[100] = { 0 };
      uint8_t       enc[100]  = { 0 };
      uint8_t       dec[100]  = { 0 };

      if (auto _n = wc_RabbitSetKey(&alice, key, nullptr); _n != 0)
        std::cerr << "wc_RabbitSetKey failed (code: " << _n << ").\n";

      if (auto _n = wc_RabbitSetKey(&bob, key, nullptr); _n != 0)
        std::cerr << "wc_RabbitSetKey failed (code: " << _n << ").\n";

      if (auto _n = wc_RabbitProcess(&alice, enc, test, 100); _n != 0)
        std::cerr << "wc_RabbitProcess failed (code: " << _n << ").\n";

      if (auto _n = wc_RabbitProcess(&bob, dec, enc, 100); _n != 0)
        std::cerr << "wc_RabbitProcess failed (code: " << _n << ").\n";

      EXPECT_EQ(memcmp(test, dec, 100), 0);
    }
  }

}
