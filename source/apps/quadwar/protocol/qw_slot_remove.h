/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef QUADWAR_PROTOCOL_QW_SLOT_REMOVE_H
#define QUADWAR_PROTOCOL_QW_SLOT_REMOVE_H

#include "../../../laplace/engine/protocol/slot_remove.h"
#include "../object/player.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_slot_remove : public slot_remove {
  public:
    ~qw_slot_remove() noexcept override = default;

    constexpr qw_slot_remove(sl::index n, sl::time time,
                             sl::index id_actor) noexcept :
        slot_remove(n, time, id_actor) { }

    inline void perform(world w) const noexcept override {
      verb(fmt(" :: event  Quadwar/slot_remove: %zu", get_actor()));

      auto r     = w.get_entity(w.get_root());
      auto slots = w.get_entity(object::root::get_slots(r));

      slots.vec_erase_by_value(get_actor(), 1);
      object::root::status_changed(r);

      w.remove(get_actor());
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return qw_slot_remove { get_index(seq), get_time(seq),
                              get_actor(seq) };
    }
  };
}

#endif
