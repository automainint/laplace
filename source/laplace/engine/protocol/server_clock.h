/*  laplace/engine/protocol/server_clock.h
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

#ifndef laplace_engine_protocol_server_clock_h
#define laplace_engine_protocol_server_clock_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class server_clock : public prime_impact {
  public:
    enum encoding_offset : size_t { n_tick_duration = 10 };

    static constexpr uint16_t id   = ids::server_clock;
    static constexpr size_t   size = 18;

    ~server_clock() final = default;

    constexpr server_clock() {
      set_encoded_size(size);
    }

    constexpr server_clock(uint64_t tick_duration_msec) {
      set_encoded_size(size);

      m_tick_duration_msec = tick_duration_msec;
    }

    constexpr server_clock(
        size_t index, uint64_t tick_duration_msec) {
      set_order({ index });
      set_encoded_size(size);

      m_tick_duration_msec = tick_duration_msec;
    }

    static constexpr auto get_tick_duration(cref_vbyte seq) {
      return rd<uint64_t>(seq, n_tick_duration);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const final {
      write_bytes(
          bytes, id, get_index64(), m_tick_duration_msec);
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return server_clock { get_index(seq),
        get_tick_duration(seq) };
    }

  private:
    uint64_t m_tick_duration_msec = 0;
  };
}

#endif
