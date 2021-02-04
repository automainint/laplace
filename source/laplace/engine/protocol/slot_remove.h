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

#ifndef __laplace__engine_protocol_slot_remove__
#define __laplace__engine_protocol_slot_remove__

namespace laplace::engine::protocol {
  class slot_remove : public sync_impact {
  public:
    enum encoding_offset : size_t { n_slot_actor = 18 };

    static constexpr uint16_t id   = ids::slot_remove;
    static constexpr size_t   size = 26;

    ~slot_remove() override = default;

    constexpr slot_remove() {
      set_size(size);
    }

    constexpr slot_remove(size_t index, uint64_t time,
                          size_t id_actor) {
      set_order({ index });
      set_time(time);
      set_actor(id_actor);
      set_size(size);
    }

    inline void perform(access::world w) const override {
      w.remove(get_actor());
    }

    inline void
    encode_to(std::span<uint8_t> bytes) const final {
      write_bytes(bytes, id, get_index64(), get_time64(),
                  get_actor64());
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return slot_remove { get_index(seq), get_time(seq),
                           get_actor(seq) };
    }
  };
}

#endif
