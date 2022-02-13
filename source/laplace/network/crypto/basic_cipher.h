/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_CRYPTO_BASIC_CIPHER_H
#define LAPLACE_NETWORK_CRYPTO_BASIC_CIPHER_H

#include "../../core/defs.h"

namespace laplace::network::crypto {
  class basic_cipher {
  public:
    basic_cipher(const basic_cipher &) = delete;
    basic_cipher(basic_cipher &&)      = delete;
    auto operator=(const basic_cipher &) -> basic_cipher & = delete;
    auto operator=(basic_cipher &&) -> basic_cipher & = delete;

    basic_cipher()          = default;
    virtual ~basic_cipher() = default;

    void set_remote_key(span_cbyte key);

    [[nodiscard]] virtual auto encrypt(span_cbyte bytes) -> vbyte;
    [[nodiscard]] virtual auto decrypt(span_cbyte bytes) -> vbyte;

    [[nodiscard]] auto is_ready() const noexcept -> bool;

    [[nodiscard]] auto get_public_key() const noexcept -> span_cbyte;
    [[nodiscard]] auto get_mutual_key() const noexcept -> span_cbyte;

    [[nodiscard]] auto get_loss_count() const noexcept -> sl::whole;

  protected:
    [[nodiscard]] virtual auto setup_remote_key(span_cbyte key)
        -> bool;
    [[nodiscard]] virtual auto setup() -> bool;

    void set_ready(bool is_ready) noexcept;

    void set_public_key(span_cbyte key) noexcept;
    void set_mutual_key(span_cbyte key) noexcept;

    void reset_loss_count() noexcept;
    void add_bytes_lost(sl::whole count) noexcept;

  private:
    bool      m_is_ready   = false;
    sl::whole m_loss_count = 0;

    vbyte m_public_key;
    vbyte m_mutual_key;
  };
}

#endif
