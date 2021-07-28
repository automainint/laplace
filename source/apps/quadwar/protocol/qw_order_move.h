/*  apps/quadwar/protocol/qw_order_move.h
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

#ifndef quadwar_protocol_qw_order_move_h
#define quadwar_protocol_qw_order_move_h

#include "../object/unit.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_order_move final : public engine::sync_prime_impact {
  public:
    enum encoding_offset : sl::index {
      n_unit     = 26,
      n_target_x = 34,
      n_target_y = 42
    };

    static constexpr auto      id   = ids::order_move;
    static constexpr sl::whole size = 50;

    ~qw_order_move() final = default;

    inline qw_order_move(sl::index id_actor, sl::index id_unit,
                         engine::intval target_x,
                         engine::intval target_y) {
      set_actor(id_actor);
      set_encoded_size(size);

      m_unit     = id_unit;
      m_target_x = target_x;
      m_target_y = target_y;
    }

    inline qw_order_move(sl::index n, uint64_t time, sl::index id_actor,
                         sl::index id_unit, engine::intval target_x,
                         engine::intval target_y) {
      set_index(n);
      set_time(time);
      set_actor(id_actor);
      set_encoded_size(size);

      m_unit     = id_unit;
      m_target_x = target_x;
      m_target_y = target_y;
    }

    inline void perform(world w) const final {
      object::unit::order_move(w, get_actor(), m_unit,
                               { m_target_x, m_target_y });
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {

      const sl::index64 unit = m_unit;
      const int64_t     x    = m_target_x;
      const int64_t     y    = m_target_y;

      serial::write_bytes(bytes, id, get_index64(), get_time64(),
                          get_actor64(), unit, x, y);
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto get_unit(span_cbyte seq) {
      return as_index(serial::rd<int64_t>(seq, n_unit));
    }

    static inline auto get_target_x(span_cbyte seq) {
      return get_intval(seq, n_target_x);
    }

    static inline auto get_target_y(span_cbyte seq) {
      return get_intval(seq, n_target_y);
    }

    static inline auto decode(span_cbyte seq) {
      return qw_order_move { get_index(seq),    get_time(seq),
                             get_actor(seq),    get_unit(seq),
                             get_target_x(seq), get_target_y(seq) };
    }

  private:
    sl::index      m_unit     = {};
    engine::intval m_target_x = {};
    engine::intval m_target_y = {};
  };
}

#endif
