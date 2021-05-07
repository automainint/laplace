/*  laplace/engine/protocol/slot_create.h
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

#ifndef laplace_engine_protocol_slot_create_h
#define laplace_engine_protocol_slot_create_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class slot_create : public sync_prime_impact {
  public:
    enum encoding_offset : size_t { n_local_flag = 26 };

    static constexpr uint16_t id   = ids::slot_create;
    static constexpr size_t   size = 27;

    ~slot_create() override = default;

    constexpr slot_create() {
      set_encoded_size(size);
    }

    constexpr slot_create(bool is_local) {
      set_encoded_size(size);

      m_is_local = is_local;
    }

    constexpr slot_create(size_t id_host_actor) {
      set_actor(id_host_actor);
      set_encoded_size(size);

      m_is_local = true;
    }

    constexpr slot_create(size_t index, uint64_t time,
                          size_t id_actor, bool is_local) {
      set_index(index);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);

      m_is_local = is_local;
    }

    inline void perform(access::world w) const override {
      w.emplace(std::make_shared<basic_entity>(), get_actor());
    }

    constexpr auto is_local() const {
      return m_is_local ? true : false;
    }

    static constexpr auto get_local_flag(span_cbyte seq) {
      return serial::rd<uint8_t>(seq, n_local_flag);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      uint8_t flag_local = is_local() ? 1u : 0u;

      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64(), flag_local);
    }

    static constexpr auto scan(span_cbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return slot_create { get_index(seq), get_time(seq),
                           get_actor(seq), get_local_flag(seq) > 0 };
    }

    /*  Alter the local status.
     *
     *  The actor is local if receiver slot
     *  actor is equal to the impact actor.
     */
    static constexpr void alter(std::span<uint8_t> seq,
                                size_t             id_slot_actor) {
      serial::wr<uint8_t>(
          seq, n_local_flag, (get_actor(seq) == id_slot_actor));
    }

  private:
    bool m_is_local = false;
  };
}

#endif
