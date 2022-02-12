/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_SLOT_REMOVE_H
#define LAPLACE_ENGINE_PROTOCOL_SLOT_REMOVE_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class slot_remove : public sync_prime_impact {
  public:
    enum encoding_offset : sl::index { n_slot_actor = 18 };

    static constexpr uint16_t  id   = ids::slot_remove;
    static constexpr sl::whole size = 26;

    ~slot_remove() noexcept override = default;

    constexpr slot_remove() noexcept {
      set_encoded_size(size);
    }

    constexpr slot_remove(sl::index n, sl::time time,
                          sl::index id_actor) noexcept {
      set_index(n);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);
    }

    inline void perform(access::world w) const noexcept override {
      w.remove(get_actor());
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64());
    }

    static constexpr auto scan(span_cbyte seq) noexcept {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return slot_remove { get_index(seq), get_time(seq),
                           get_actor(seq) };
    }
  };
}

#endif
