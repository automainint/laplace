/*  laplace/engine/protocol/ping.h
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

#ifndef laplace_engine_protocol_ping_h
#define laplace_engine_protocol_ping_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  template <uint16_t id_>
  class basic_ping final : public prime_impact {
  public:
    enum encoding_offset : size_t { n_ping_time = 2 };

    static constexpr uint16_t id   = id_;
    static constexpr size_t   size = 10;

    ~basic_ping() final = default;

    constexpr basic_ping() {
      this->set_encoded_size(size);
    }

    constexpr basic_ping(uint64_t time_msec) {
      this->set_encoded_size(size);
      this->m_time_msec = time_msec;
    }

    static constexpr auto get_ping_time(span_cbyte seq) {
      return serial::rd<uint64_t>(seq, n_ping_time);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(bytes, id, this->m_time_msec);
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return basic_ping { get_ping_time(seq) };
    }

  private:
    uint64_t m_time_msec = 0;
  };

  using ping_request  = basic_ping<ids::ping_request>;
  using ping_response = basic_ping<ids::ping_response>;
}

#endif
