/*  laplace/engine/protocol/server_idle.h
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

#ifndef laplace_engine_protocol_server_idle_h
#define laplace_engine_protocol_server_idle_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class server_idle final : public prime_impact {
  public:
    static constexpr uint16_t id   = ids::server_idle;
    static constexpr size_t   size = 18;

    ~server_idle() final = default;

    constexpr server_idle() {
      set_encoded_size(size);
    }

    constexpr server_idle(uint64_t index, uint64_t time_msec) {
      set_index(index);
      set_time(time_msec);
      set_encoded_size(size);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(bytes, id, get_index64(), get_time64());
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static constexpr auto get_idle_index(span_cbyte seq) {
      return get_index_unsafe(seq);
    }

    static constexpr auto get_idle_time(span_cbyte seq) {
      return get_time_unsafe(seq);
    }

    static inline auto decode(span_cbyte seq) {
      return server_idle { get_idle_index(seq), get_idle_time(seq) };
    }
  };
}

#endif
