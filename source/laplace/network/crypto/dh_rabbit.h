/*  laplace/network/crypto/dh_rabbit.h
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

#ifndef __laplace__network_crypto_dh_rabbit__
#define __laplace__network_crypto_dh_rabbit__

#include "../../core/defs.h"

#define WC_NO_HARDEN
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/rabbit.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

namespace laplace::network::crypto {
  class dh_rabbit {
  public:
    static constexpr size_t loss_limit = 0x1000;

    dh_rabbit(const dh_rabbit &) = delete;
    auto operator=(const dh_rabbit &) -> dh_rabbit & = delete;

    dh_rabbit();
    dh_rabbit(dh_rabbit &&r) noexcept;
    auto operator=(dh_rabbit &&r) noexcept -> dh_rabbit &;

    ~dh_rabbit();

    [[nodiscard]] auto get_public_key() const -> cref_vbyte;
    [[nodiscard]] auto get_mutual_key() const -> cref_vbyte;

    void set_remote_key(cref_vbyte key);

    [[nodiscard]] auto encrypt(cref_vbyte bytes) -> vbyte;
    [[nodiscard]] auto decrypt(cref_vbyte bytes) -> vbyte;

    [[nodiscard]] auto is_ready() -> bool;

  private:
    void init();
    auto pass(const size_t offset) -> bool;
    auto check_sum(cref_vbyte bytes) -> uint16_t;
    auto check(cref_vbyte bytes) -> bool;
    auto decrypt_chunk(cref_vbyte chunk) -> vbyte;

    enum _offset : size_t { //
      n_offset     = 0,     //
      n_size       = 8,     //
      n_header     = 16,    //
      n_check_sum  = 4,     //
      n_check_null = 2,     //
      n_overhead   = n_header + n_check_sum
    };

    using sum_type = uint16_t;

    static constexpr size_t key_size = 256;

    using key_bytes = std::array<uint8_t, key_size>;

    bool m_is_ok    = true;
    bool m_is_ready = false;

    WC_RNG m_random;
    DhKey  m_key;

    uint32_t m_public_size  = key_size;
    uint32_t m_private_size = key_size;
    uint32_t m_mutual_size  = 0;

    key_bytes m_public  = { 0 };
    key_bytes m_private = { 0 };
    key_bytes m_mutual  = { 0 };

    size_t m_enc_offset = 0;
    size_t m_dec_offset = 0;

    Rabbit m_encrypt = { 0 };
    Rabbit m_decrypt = { 0 };
  };
}

#endif
