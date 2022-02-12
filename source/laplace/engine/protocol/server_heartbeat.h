/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_SERVER_HEARTBEAT_H
#define LAPLACE_ENGINE_PROTOCOL_SERVER_HEARTBEAT_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class server_heartbeat final : public prime_impact {
  public:
    static constexpr uint16_t  id   = ids::server_heartbeat;
    static constexpr sl::whole size = 18;

    ~server_heartbeat() noexcept final = default;

    constexpr server_heartbeat() noexcept {
      set_encoded_size(size);
    }

    constexpr server_heartbeat(sl::index n,
                               sl::time  time_msec) noexcept {
      set_index(n);
      set_time(time_msec);
      set_encoded_size(size);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(bytes, id, get_index64(), get_time64());
    }

    static constexpr auto scan(span_cbyte seq) noexcept -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static constexpr auto get_idle_index(span_cbyte seq) noexcept {
      return get_index_unsafe(seq);
    }

    static constexpr auto get_idle_time(span_cbyte seq) noexcept {
      return get_time_unsafe(seq);
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return server_heartbeat { get_idle_index(seq),
                                get_idle_time(seq) };
    }
  };
}

#endif
