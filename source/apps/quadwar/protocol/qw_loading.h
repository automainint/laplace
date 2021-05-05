/*  apps/quadwar/protocol/qw_loading.h
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

#ifndef quadwar_protocol_qw_loading_h
#define quadwar_protocol_qw_loading_h

#include "../object/game_clock.h"
#include "../object/landscape.h"
#include "../object/root.h"
#include "../object/unit.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_loading final : public engine::sync_prime_impact {
  public:
    enum encoding_offset : size_t {
      n_map_size     = 10,
      n_player_count = 18,
      n_unit_count   = 26
    };

    static constexpr uint16_t id   = ids::server_loading;
    static constexpr size_t   size = 34;

    ~qw_loading() final = default;

    inline qw_loading(       //
        size_t map_size,     //
        size_t player_count, //
        size_t unit_count) {

      set_encoded_size(size);

      m_map_size     = map_size;
      m_player_count = player_count;
      m_unit_count   = unit_count;
    }

    inline qw_loading(       //
        size_t index,        //
        size_t map_size,     //
        size_t player_count, //
        size_t unit_count) {

      set_order({ index });
      set_encoded_size(size);

      m_map_size     = map_size;
      m_player_count = player_count;
      m_unit_count   = unit_count;
    }

    inline void perform(world w) const final {
      verb(" :: event  Quadwar/loading");

      auto r = w.get_entity(w.get_root());

      object::root::loading(r);

      w.spawn(std::make_shared<object::game_clock>(),
              engine::id_undefined);

      object::landscape::create_maze(
          w, m_map_size, m_map_size, m_player_count);

      r.adjust();

      object::unit::spawn_start_units(w, m_unit_count);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(                       //
          bytes,                                 //
          id,                                    //
          get_index64(),                         //
          static_cast<uint64_t>(m_map_size),     //
          static_cast<uint64_t>(m_player_count), //
          static_cast<uint64_t>(m_unit_count));
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto get_map_size(span_cbyte seq) {
      return as_index(serial::rd<uint64_t>(seq, n_map_size));
    }

    static inline auto get_player_count(span_cbyte seq) {
      return as_index(serial::rd<uint64_t>(seq, n_player_count));
    }

    static inline auto get_unit_count(span_cbyte seq) {
      return as_index(serial::rd<uint64_t>(seq, n_unit_count));
    }

    static inline auto decode(span_cbyte seq) {
      return qw_loading            //
          { get_index(seq),        //
            get_map_size(seq),     //
            get_player_count(seq), //
            get_unit_count(seq) };
    }

  private:
    size_t m_map_size;
    size_t m_player_count;
    size_t m_unit_count;
  };
}

#endif
