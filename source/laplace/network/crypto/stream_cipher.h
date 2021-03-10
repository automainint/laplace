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
    [[nodiscard]] auto scan(cref_vbyte data) const -> bool;

    enum encoding_offset : size_t {
      n_offset = 0,
      n_sum    = 8,
      n_size   = 16,
      n_data   = 24
    };

    static constexpr size_t block_size = 64;

    size_t m_enc_offset = 0;
    size_t m_dec_offset = 0;
  };
}

#endif
