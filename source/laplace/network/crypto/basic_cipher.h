/*  laplace/network/crypto/basic_cipher.h
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

#ifndef laplace_network_crypto_basic_cipher_h
#define laplace_network_crypto_basic_cipher_h

#include "../../core/defs.h"

namespace laplace::network::crypto {
  class basic_cipher {
  public:
    static constexpr bool default_verbose = false;

    basic_cipher(const basic_cipher &) = delete;
    basic_cipher(basic_cipher &&)      = delete;
    auto operator=(const basic_cipher &) -> basic_cipher & = delete;
    auto operator=(basic_cipher &&) -> basic_cipher & = delete;

    basic_cipher()          = default;
    virtual ~basic_cipher() = default;

    void set_remote_key(cref_vbyte key);
    void set_verbose(bool is_verbose) noexcept;

    [[nodiscard]] virtual auto encrypt(cref_vbyte bytes) -> vbyte;
    [[nodiscard]] virtual auto decrypt(cref_vbyte bytes) -> vbyte;

    [[nodiscard]] auto is_ready() const noexcept -> bool;

    [[nodiscard]] auto get_public_key() const noexcept -> cref_vbyte;
    [[nodiscard]] auto get_mutual_key() const noexcept -> cref_vbyte;

  protected:
    void verb_error(std::string_view sender, std::string_view message);

    [[nodiscard]] virtual auto setup_remote_key(cref_vbyte key) -> bool;
    [[nodiscard]] virtual auto setup() -> bool;

    void set_ready(bool is_ready) noexcept;

    void set_private_key(cref_vbyte key) noexcept;
    void set_public_key(cref_vbyte key) noexcept;
    void set_mutual_key(cref_vbyte key) noexcept;

    [[nodiscard]] auto get_private_key() const noexcept -> cref_vbyte;

  private:
    bool m_is_ready   = false;
    bool m_is_verbose = default_verbose;

    vbyte m_private_key;
    vbyte m_public_key;
    vbyte m_mutual_key;
  };
}

#endif
