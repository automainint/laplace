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

#ifndef __laplace__engine_protocol_server_idle__
#define __laplace__engine_protocol_server_idle__

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class server_idle final : public prime_impact {
  public:
    enum encoding_offset : size_t { n_idle_time = 2 };

    static constexpr uint16_t id   = ids::server_idle;
    static constexpr size_t   size = 10;

    ~server_idle() final = default;

    constexpr server_idle() {
      set_encoded_size(size);
    }

    constexpr server_idle(uint64_t time_msec) {
      set_time(time_msec);
      set_encoded_size(size);
    }

    static constexpr auto get_idle_time(cref_vbyte seq) {
      return rd<uint64_t>(seq, n_idle_time);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      write_bytes(bytes, id, get_time64());
    }

    static constexpr auto scan(cref_vbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return server_idle { get_idle_time(seq) };
    }
  };
}

#endif
