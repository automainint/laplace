/*  apps/quadwar/qw_player_name.h
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

#ifndef __quadwar__qw_player_name__
#define __quadwar__qw_player_name__

#include "../../laplace/engine/protocol/slot_create.h"
#include "ids.h"
#include "player.h"
#include "root.h"

namespace quadwar_app {
  using namespace laplace;

  class qw_player_name : public engine::sync_prime_impact {
  public:
    enum encoding_offset { n_name = 26 };

    static constexpr auto id = ids::player_name;

    ~qw_player_name() override = default;

    inline qw_player_name(std::u8string_view name) {
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline qw_player_name(size_t             id_host_actor,
                          std::u8string_view name) {
      set_actor(id_host_actor);
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline qw_player_name(size_t index, uint64_t time,
                          size_t             id_actor,
                          std::u8string_view name) {
      set_order({ index });
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline void
    perform(engine::access::world w) const override {
      verb(" :: event  Quadwar/player_name");

      player::set_name(w.get_entity(get_actor()), m_name);
      root::status_changed(w.get_root());
    }

    inline void
    encode_to(std::span<uint8_t> bytes) const final {
      write_bytes(bytes, id, get_index64(), get_time64(),
                  get_actor64(), std::u8string_view(m_name));
    }

    static constexpr auto scan(cref_vbyte seq) -> bool {
      return seq.size() >= n_name && get_id(seq) == id;
    }

    static inline auto get_name(cref_vbyte seq) {
      return get_string(seq, n_name);
    }

    static inline auto decode(cref_vbyte seq) {
      return qw_player_name { get_index(seq), get_time(seq),
                              get_actor(seq), get_name(seq) };
    }

  private:
    std::u8string m_name;
  };
}

#endif
