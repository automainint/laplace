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
    void set_verbose(bool is_verbose) noexcept;
    void set_cipher(std::unique_ptr<crypto::basic_cipher> cipher);
    void set_remote_key(cref_vbyte key);

    [[nodiscard]] auto pack(std::span<const cref_vbyte> data) -> vbyte;
    [[nodiscard]] auto unpack(cref_vbyte data) -> std::vector<vbyte>;

    [[nodiscard]] auto encode(std::span<const cref_vbyte> data)
        -> vbyte;
    [[nodiscard]] auto decode(cref_vbyte data) -> std::vector<vbyte>;

    [[nodiscard]] auto get_public_key() const noexcept -> cref_vbyte;
    [[nodiscard]] auto get_mutual_key() const noexcept -> cref_vbyte;

    [[nodiscard]] auto is_encrypted() const noexcept -> bool;

    [[nodiscard]] auto get_loss_count() const noexcept -> size_t;

    [[nodiscard]] static auto check_sum(cref_vbyte data) -> uint64_t;

    template <typename cipher_>
    void setup_cipher() {
      this->set_cipher(std::make_unique<cipher_>());
    }

  private:
    [[nodiscard]] auto pack_internal(     //
        std::span<const cref_vbyte> data, //
        const uint16_t              mark) -> vbyte;

    [[nodiscard]] auto unpack_internal( //
        cref_vbyte     data,            //
        const uint16_t mark) -> std::vector<vbyte>;

    [[nodiscard]] auto scan( //
        cref_vbyte data,     //
        uint16_t   mark) const noexcept -> size_t;

    static constexpr uint16_t mark_plain     = 0;
    static constexpr uint16_t mark_encrypted = 1;

    enum encoding_offset : size_t {
      n_mark = 0,
      n_sum  = 2,
      n_size = 10,
      n_data = 18
    };

    std::unique_ptr<crypto::basic_cipher> m_cipher;

    bool   m_verbose    = false;
    size_t m_loss_count = 0;
  };
}

#endif
