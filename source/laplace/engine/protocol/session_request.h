/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_SESSION_REQUEST_H
#define LAPLACE_ENGINE_PROTOCOL_SESSION_REQUEST_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class session_request final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_cipher = 2, n_key = 4 };

    static constexpr sl::whole max_key_size = 1024;
    static constexpr uint16_t  id           = ids::session_request;

    using key_type = std::array<uint8_t, max_key_size>;

    ~session_request() noexcept final = default;

    constexpr session_request() noexcept {
      set_encoded_size(n_key);
    }

    constexpr session_request(uint16_t   cipher_id,
                              span_cbyte key) noexcept {
      m_cipher   = cipher_id;
      m_key_size = std::min<sl::whole>(key.size(), max_key_size);

      set_encoded_size(n_key + m_key_size);

      std::copy(key.begin(), key.begin() + m_key_size, m_key.begin());
    }

    static constexpr auto get_cipher(span_cbyte seq) noexcept {
      return serial::rd<uint16_t>(seq, n_cipher);
    }

    static constexpr auto get_key(span_cbyte seq) noexcept
        -> span_cbyte {
      if (seq.size() > n_key) {
        return { seq.begin() + n_key, seq.end() };
      }

      return {};
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(
          bytes, id, m_cipher,
          std::span<const uint8_t>(m_key.data(), m_key_size));
    }

    static constexpr auto scan(span_cbyte seq) noexcept -> bool {
      return seq.size() >= n_key && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return session_request { get_cipher(seq), get_key(seq) };
    }

  private:
    uint16_t  m_cipher   = {};
    sl::whole m_key_size = {};
    key_type  m_key      = {};
  };
}

#endif
