/*  apps/quadwar/protocol/qw_player_name.h
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

#ifndef quadwar_protocol_qw_player_name_h
#define quadwar_protocol_qw_player_name_h

#include "../object/player.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_player_name final : public engine::sync_prime_impact {
  public:
    enum encoding_offset : sl::index { n_name = 26 };

    static constexpr auto id = ids::player_name;

    ~qw_player_name() final = default;

    inline qw_player_name(std::u8string_view name) {
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline qw_player_name(sl::index          id_host_actor,
                          std::u8string_view name) {

      set_actor(id_host_actor);
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline qw_player_name(sl::index          n,
                          sl::time           time,
                          sl::index          id_actor,
                          std::u8string_view name) {

      set_index(n);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(n_name + name.size());

      m_name = name;
    }

    inline void perform(world w) const final {
      verb(" :: event  Quadwar/player_name");

      const auto id_actor = get_actor();
      const auto id_root  = w.get_root();

      object::player::set_name(w.get_entity(id_actor), m_name);
      object::root::status_changed(w.get_entity(id_root));
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {

      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64(), std::u8string_view(m_name));
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() >= n_name && get_id(seq) == id;
    }

    static inline auto get_name(span_cbyte seq) {
      return get_string(seq, n_name);
    }

    static inline auto decode(span_cbyte seq) {
      return qw_player_name { get_index(seq), get_time(seq),
                              get_actor(seq), get_name(seq) };
    }

  private:
    std::u8string m_name;
  };
}

#endif
