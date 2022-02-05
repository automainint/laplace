/*  Copyright (c) 2022 Mitya Selivanov
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
    enum encryption_tag { plain, encrypted };

    void set_verbose(bool is_verbose) noexcept;
    void set_cipher(std::unique_ptr<crypto::basic_cipher> cipher);
    void set_remote_key(span_cbyte key);

    [[nodiscard]] static auto pack(std::span<span_cbyte const> data)
        -> vbyte;

    [[nodiscard]] auto encode(std::span<span_cbyte const> data,
                              encryption_tag _enc = encrypted)
        -> vbyte;

    [[nodiscard]] auto decode(span_cbyte data) -> sl::vector<vbyte>;

    [[nodiscard]] auto get_public_key() const noexcept -> span_cbyte;

    [[nodiscard]] auto is_encrypted() const noexcept -> bool;

    [[nodiscard]] auto get_loss_count() const noexcept -> sl::whole;

    [[nodiscard]] static auto get_data_overhead() noexcept
        -> sl::whole;

    [[nodiscard]] static auto check_sum(span_cbyte data) -> uint64_t;

    template <typename cipher_>
    void setup_cipher() {
      this->set_cipher(std::make_unique<cipher_>());
    }

  private:
    [[nodiscard]] static auto pack_internal(
        std::span<const span_cbyte> data, uint16_t mark) -> vbyte;

    [[nodiscard]] auto unpack_internal(span_cbyte data, uint16_t mark)
        -> std::vector<vbyte>;

    [[nodiscard]] auto scan(span_cbyte data,
                            uint16_t   mark) const noexcept
        -> sl::whole;

    static constexpr uint16_t mark_plain     = 0;
    static constexpr uint16_t mark_encrypted = 1;

    enum encoding_offset : sl::whole {
      n_mark = 0,
      n_sum  = 2,
      n_size = 10,
      n_data = 18
    };

    std::unique_ptr<crypto::basic_cipher> m_cipher;

    bool      m_verbose    = false;
    sl::whole m_loss_count = 0;
  };
}

#endif
