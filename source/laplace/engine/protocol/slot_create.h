/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_SLOT_CREATE_H
#define LAPLACE_ENGINE_PROTOCOL_SLOT_CREATE_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class slot_create : public sync_prime_impact {
  public:
    enum encoding_offset : sl::index { n_local_flag = 26 };

    static constexpr uint16_t  id   = ids::slot_create;
    static constexpr sl::whole size = 27;

    ~slot_create() noexcept override = default;

    constexpr slot_create() noexcept {
      set_encoded_size(size);
    }

    constexpr slot_create(bool is_local) noexcept {
      set_encoded_size(size);

      m_is_local = is_local;
    }

    constexpr slot_create(sl::index id_host_actor) noexcept {
      set_actor(id_host_actor);
      set_encoded_size(size);

      m_is_local = true;
    }

    constexpr slot_create(sl::index n, sl::time time,
                          sl::index id_actor,
                          bool      is_local) noexcept {
      set_index(n);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);

      m_is_local = is_local;
    }

    inline void perform(access::world w) const noexcept override {
      w.emplace(std::make_shared<basic_entity>(), get_actor());
    }

    constexpr auto is_local() const noexcept {
      return m_is_local ? true : false;
    }

    static constexpr auto get_local_flag(span_cbyte seq) noexcept {
      return serial::rd<uint8_t>(seq, n_local_flag);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      uint8_t flag_local = is_local() ? 1u : 0u;

      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64(), flag_local);
    }

    static constexpr auto scan(span_cbyte seq) noexcept {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return slot_create { get_index(seq), get_time(seq),
                           get_actor(seq), get_local_flag(seq) > 0 };
    }

    /*  Alter the local status.
     *
     *  The actor is local if receiver slot
     *  actor is equal to the impact actor.
     */
    static constexpr void alter(std::span<uint8_t> seq,
                                sl::index id_slot_actor) noexcept {
      serial::wr<uint8_t>(seq, n_local_flag,
                          (get_actor(seq) == id_slot_actor));
    }

  private:
    bool m_is_local = false;
  };
}

#endif
