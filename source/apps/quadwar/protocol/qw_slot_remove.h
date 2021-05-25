/*  apps/quadwar/protocol/qw_slot_remove.h
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

#ifndef quadwar_protocol_qw_slot_remove_h
#define quadwar_protocol_qw_slot_remove_h

#include "../../../laplace/engine/protocol/slot_remove.h"
#include "../object/player.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_slot_remove : public slot_remove {
  public:
    ~qw_slot_remove() override = default;

    constexpr qw_slot_remove(sl::index n, uint64_t time,
                             sl::index id_actor) :
        slot_remove(n, time, id_actor) { }

    inline void perform(world w) const override {
      verb(fmt(" :: event  Quadwar/slot_remove: %zu", get_actor()));

      object::root::slot_remove(
          w.get_entity(w.get_root()), get_actor());

      w.remove(get_actor());
    }

    static inline auto decode(span_cbyte seq) {
      return qw_slot_remove { get_index(seq), get_time(seq),
                              get_actor(seq) };
    }
  };
}

#endif
