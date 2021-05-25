/*  apps/quadwar/protocol/qw_slot_create.h
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

#ifndef quadwar_protocol_qw_slot_create_h
#define quadwar_protocol_qw_slot_create_h

#include "../../../laplace/engine/protocol/slot_create.h"
#include "../object/player.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  class qw_slot_create : public slot_create {
  public:
    ~qw_slot_create() override = default;

    constexpr qw_slot_create(sl::index index, uint64_t time,
                             sl::index id_actor, bool is_local) :
        slot_create(index, time, id_actor, is_local) { }

    inline void perform(world w) const override {
      verb(fmt(" :: event  Quadwar/slot_create: %zu %s", get_actor(),
               is_local() ? "local" : "remote"));

      w.emplace(
          std::make_shared<object::player>(is_local()), get_actor());

      object::root::slot_create(
          w.get_entity(w.get_root()), get_actor());
    }

    static inline auto decode(span_cbyte seq) {
      return qw_slot_create { get_index(seq), get_time(seq),
                              get_actor(seq), get_local_flag(seq) > 0 };
    }
  };
}

#endif
