/*  laplace/network/crypto/stream_cipher.h
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

#ifndef laplace_network_crypto_stream_cipher_h
#define laplace_network_crypto_stream_cipher_h

#include "basic_cipher.h"
#include <tuple>

namespace laplace::network::crypto {
  class stream_cipher : virtual public basic_cipher {
  public:
    ~stream_cipher() override = default;

    [[nodiscard]] auto encrypt(cref_vbyte bytes) -> vbyte override;
    [[nodiscard]] auto decrypt(cref_vbyte bytes) -> vbyte override;

  protected:
    auto setup() -> bool override;

    [[nodiscard]] virtual auto do_encrypt( //
        cref_vbyte         src,            //
        std::span<uint8_t> dst) -> bool;

    [[nodiscard]] virtual auto do_decrypt( //
        cref_vbyte         src,            //
        std::span<uint8_t> dst) -> bool;

    [[nodiscard]] virtual auto rewind_decryption() -> bool;

    [[nodiscard]] virtual auto pass_decryption( //
        size_t offset) -> bool;

  private:
    static constexpr size_t block_size = 64;

    enum encoding_offset : size_t {
      n_chunk_offset    = 0,
      n_chunk_check_sum = 8,
      n_chunk_data      = 16,

      n_pack_size      = 0,
      n_pack_check_sum = 8,
      n_pack_data      = 16
    };

    [[nodiscard]] auto data_pack( //
        cref_vbyte bytes) -> vbyte;

    [[nodiscard]] auto data_unpack( //
        cref_vbyte bytes) -> vbyte;

    [[nodiscard]] auto chunk_scan( //
        cref_vbyte bytes) -> bool;

    [[nodiscard]] auto chunk_pack( //
        const size_t offset,       //
        cref_vbyte   bytes) -> vbyte;

    [[nodiscard]] auto chunk_unpack( //
        cref_vbyte bytes) -> std::tuple<vbyte, size_t>;

    [[nodiscard]] auto chunk_encrypt( //
        cref_vbyte bytes) -> vbyte;

    [[nodiscard]] auto rewind( //
        const size_t offset) -> bool;

    [[nodiscard]] auto chunk_decrypt( //
        const size_t offset,          //
        cref_vbyte   bytes) -> vbyte;

    size_t m_enc_offset = 0;
    size_t m_dec_offset = 0;
  };
}

#endif
