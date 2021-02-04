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

#ifndef __laplace__engine_protocol_debug__
#define __laplace__engine_protocol_debug__

#include "../basic_impact.h"

namespace laplace::engine::protocol {
  class debug final : public sync_impact {
  public:
    enum encoding_offset { n_value = 18 };

    static constexpr uint16_t id = ids::debug;
    static constexpr size_t size = 26;

    ~debug() final = default;

    constexpr debug() {
      set_size(size);
    }

    constexpr debug(int64_t value) {
      set_size(size);

      m_value = value;
    }

    constexpr debug(
        size_t index, uint64_t time, int64_t value) {
      set_order({ index });
      set_time(time);
      set_size(size);

      m_value = value;
    }

    static constexpr auto get_value(cref_vbyte seq) {
      return rd<int64_t>(seq, n_value);
    }

    void perform(access::world w) const final;

    inline void encode_to(
        std::span<uint8_t> bytes) const final {
      write_bytes(
          bytes, id, get_index64(), get_time64(), m_value);
    }

    static constexpr auto scan(cref_vbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return debug { get_index(seq), get_time(seq),
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
