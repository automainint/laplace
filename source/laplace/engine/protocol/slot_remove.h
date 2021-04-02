/*  laplace/engine/protocol/slot_remove.h
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

#ifndef laplace_engine_protocol_slot_remove_h
#define laplace_engine_protocol_slot_remove_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class slot_remove : public sync_prime_impact {
  public:
    enum encoding_offset : size_t { n_slot_actor = 18 };

    static constexpr uint16_t id   = ids::slot_remove;
    static constexpr size_t   size = 26;

    ~slot_remove() override = default;

    constexpr slot_remove() {
      set_encoded_size(size);
    }

    constexpr slot_remove(size_t index, uint64_t time, size_t id_actor) {
      set_order({ index });
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);
    }

    inline void perform(access::world w) const override {
      w.remove(get_actor());
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(
          bytes, id, get_index64(), get_time64(), get_actor64());
    }

    static constexpr auto scan(span_cbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return slot_remove { get_index(seq), get_time(seq),
                           get_actor(seq) };
    }
  };
}

#endif
