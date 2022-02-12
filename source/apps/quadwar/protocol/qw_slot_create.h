/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef QUADWAR_PROTOCOL_QW_SLOT_CREATE_H
#define QUADWAR_PROTOCOL_QW_SLOT_CREATE_H

#include "../../../laplace/engine/protocol/slot_create.h"
#include "../object/player.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_slot_create : public slot_create {
  public:
    ~qw_slot_create() noexcept override = default;

    constexpr qw_slot_create(sl::index index, sl::time time,
                             sl::index id_actor,
                             bool      is_local) noexcept :
        slot_create(index, time, id_actor, is_local) { }

    inline void perform(world w) const noexcept override {
      verb(fmt(" :: event  Quadwar/slot_create: %zu %s", get_actor(),
               is_local() ? "local" : "remote"));

      w.emplace(std::make_shared<object::player>(is_local()),
                get_actor());

      auto r     = w.get_entity(w.get_root());
      auto slots = w.get_entity(object::root::get_slots(r));

      slots.vec_add(get_actor());
      object::root::status_changed(r);
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return qw_slot_create { get_index(seq), get_time(seq),
                              get_actor(seq),
                              get_local_flag(seq) > 0 };
    }
  };
}

#endif
