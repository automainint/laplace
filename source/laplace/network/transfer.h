/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_TRANSFER_H
#define LAPLACE_NETWORK_TRANSFER_H

#include "crypto/basic_cipher.h"

namespace laplace::network {
  class transfer {
  public:
    log_handler log = get_global_log();
    
    void set_verbose(bool is_verbose) noexcept;
    void set_cipher(
        std::unique_ptr<crypto::basic_cipher> cipher) noexcept;
    void set_remote_key(span_cbyte key) noexcept;

    void enable_encryption(bool is_enabled) noexcept;

    [[nodiscard]] static auto pack(
        std::span<span_cbyte const> data) noexcept -> vbyte;

    [[nodiscard]] auto encode(
        std::span<span_cbyte const> data) noexcept -> vbyte;

    [[nodiscard]] auto decode(span_cbyte data) noexcept
        -> sl::vector<vbyte>;

    [[nodiscard]] auto get_public_key() const noexcept -> span_cbyte;

    [[nodiscard]] auto is_cipher_set() const noexcept -> bool;
    [[nodiscard]] auto is_encryption_enabled() const noexcept -> bool;

    [[nodiscard]] auto get_loss_count() const noexcept -> sl::whole;

    [[nodiscard]] static auto get_data_overhead() noexcept
        -> sl::whole;

    [[nodiscard]] static auto check_sum(span_cbyte data) noexcept
        -> uint64_t;

    template <typename cipher_>
    void setup_cipher() noexcept {
      this->set_cipher(std::make_unique<cipher_>());
    }

    [[nodiscard]] static auto wrap(span_cbyte arg0,
                                   auto... args) noexcept
        -> sl::vector<span_cbyte>;

    [[nodiscard]] static auto wrap(
        sl::vector<vbyte> const &reqs) noexcept
        -> sl::vector<span_cbyte>;

  private:
    static void _append(sl::vector<span_cbyte> &v) noexcept;
    static void _append(sl::vector<span_cbyte> &v, auto arg0,
                        auto... args) noexcept;

    [[nodiscard]] static auto pack_internal(
        std::span<const span_cbyte> data, uint16_t mark) noexcept
        -> vbyte;

    [[nodiscard]] auto unpack_internal(span_cbyte data,
                                       uint16_t   mark) noexcept
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
    bool      m_encryption = false;
    sl::whole m_loss_count = 0;
  };
}

#include "transfer.impl.h"

#endif
