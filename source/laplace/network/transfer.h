/*  laplace/network/transfer.h
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

#ifndef laplace_network_transfer_h
#define laplace_network_transfer_h

#include "crypto/basic_cipher.h"

namespace laplace::network {
  class transfer {
  public:
    void set_cipher(std::unique_ptr<crypto::basic_cipher> cipher);
    void set_remote_key(cref_vbyte key);

    [[nodiscard]] auto pack(cref_vbyte data) -> vbyte;
    [[nodiscard]] auto unpack(cref_vbyte data) -> vbyte;

    [[nodiscard]] auto encode(cref_vbyte data) -> vbyte;
    [[nodiscard]] auto decode(cref_vbyte data) -> vbyte;

    [[nodiscard]] auto get_public_key() const noexcept -> cref_vbyte;
    [[nodiscard]] auto get_mutual_key() const noexcept -> cref_vbyte;

    [[nodiscard]] auto is_encrypted() const noexcept -> bool;

    [[nodiscard]] static auto check_sum(cref_vbyte data) -> uint64_t;

    template <typename cipher_>
    void setup_cipher() {
      this->set_cipher(std::make_unique<cipher_>());
    }

  private:
    enum encodng_offset : size_t {
      n_size      = 0,
      n_check_sum = 8,
      n_data      = 16
    };

    std::unique_ptr<crypto::basic_cipher> m_cipher;
  };
}

#endif
