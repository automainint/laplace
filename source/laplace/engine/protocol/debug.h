/*  laplace/engine/protocol/debug.h
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

#ifndef laplace_engine_protocol_debug_h
#define laplace_engine_protocol_debug_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class debug final : public sync_prime_impact {
  public:
    enum encoding_offset : sl::index { n_value = 26 };

    static constexpr uint16_t  id   = ids::debug;
    static constexpr sl::whole size = 34;

    ~debug() final = default;

    constexpr debug() {
      set_encoded_size(size);
    }

    constexpr debug(int64_t value) {
      set_encoded_size(size);

      m_value = value;
    }

    constexpr debug(sl::index n,
                    uint64_t  time,
                    sl::index id_actor,
                    int64_t   value) {
      set_index(n);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);

      m_value = value;
    }

    static constexpr auto get_value(span_cbyte seq) {
      return serial::rd<int64_t>(seq, n_value);
    }

    void perform(access::world w) const final;

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64(), m_value);
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return debug { get_index(seq), get_time(seq), get_actor(seq),
                     get_value(seq) };
    }

  private:
    class debug_root : public basic_entity {
    public:
      debug_root();
    };

    int64_t m_value = 0;
  };
}

#endif
