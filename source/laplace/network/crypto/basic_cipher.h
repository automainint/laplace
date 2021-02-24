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
    static constexpr bool   default_verbose = false;
    static constexpr size_t max_key_size    = 1024;

    basic_cipher(const basic_cipher &) = delete;
    basic_cipher(basic_cipher &&)      = delete;
    auto operator=(const basic_cipher &) -> basic_cipher & = delete;
    auto operator=(basic_cipher &&) -> basic_cipher & = delete;

    basic_cipher()          = default;
    virtual ~basic_cipher() = default;

    void set_remote_key(cref_vbyte key);

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
    using key_bytes = std::array<uint8_t, max_key_size>;

    bool m_is_ready   = false;
    bool m_is_verbose = default_verbose;

    size_t m_private_key_size = 0;
    size_t m_public_key_size  = 0;
    size_t m_mutual_key_size  = 0;

    key_bytes m_private_key;
    key_bytes m_public_key;
    key_bytes m_mutual_key;
  };
}

#endif
