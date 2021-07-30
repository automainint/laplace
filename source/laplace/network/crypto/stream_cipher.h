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

    [[nodiscard]] auto encrypt(span_cbyte bytes) -> vbyte override;
    [[nodiscard]] auto decrypt(span_cbyte bytes) -> vbyte override;

  protected:
    [[nodiscard]] virtual auto do_encrypt(span_cbyte         src,
                                          std::span<uint8_t> dst)
        -> bool;

    [[nodiscard]] virtual auto do_decrypt(span_cbyte         src,
                                          std::span<uint8_t> dst)
        -> bool;

    [[nodiscard]] virtual auto rewind_decryption() -> bool;

    [[nodiscard]] virtual auto pass_decryption(sl::index offset)
        -> bool;

  private:
    [[nodiscard]] auto scan(span_cbyte data) const -> bool;

    enum encoding_offset : sl::index {
      n_offset = 0,
      n_sum    = 8,
      n_size   = 16,
      n_data   = 24
    };

    static const sl::whole block_size;
    static const sl::whole max_offset_change;

    sl::index m_enc_offset = 0;
    sl::index m_dec_offset = 0;
  };
}

#endif
