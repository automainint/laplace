/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_SESSION_TOKEN_H
#define LAPLACE_ENGINE_PROTOCOL_SESSION_TOKEN_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class session_token final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_token = 2 };

    static constexpr sl::whole max_token_size = 128;
    static constexpr uint16_t  id             = ids::session_token;

    using token_type = std::array<uint8_t, max_token_size>;

    ~session_token() noexcept final = default;

    constexpr session_token() noexcept {
      set_encoded_size(n_token);
    }

    constexpr session_token(span_cbyte token) noexcept {
      m_token_size = std::min<sl::whole>(token.size(),
                                         max_token_size);

      set_encoded_size(n_token + m_token_size);

      std::copy(token.begin(), token.begin() + m_token_size,
                m_token.begin());
    }

    static constexpr auto get_token(span_cbyte seq) noexcept
        -> span_cbyte {
      if (seq.size() > n_token) {
        return { seq.begin() + n_token, seq.end() };
      }

      return {};
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(
          bytes, id,
          std::span<const uint8_t>(m_token.data(), m_token_size));
    }

    static constexpr auto scan(span_cbyte seq) noexcept -> bool {
      return seq.size() >= n_token && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return session_token { get_token(seq) };
    }

  private:
    sl::whole  m_token_size = {};
    token_type m_token      = {};
  };
}

#endif
