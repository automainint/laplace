/*  apps/quadwar/qw_slot_remove.h
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

#ifndef __quadwar__qw_slot_remove__
#define __quadwar__qw_slot_remove__

#include "../../laplace/engine/protocol/slot_remove.h"
#include "player.h"
#include "root.h"

namespace quadwar_app {
  using namespace laplace;

  class qw_slot_remove : public engine::protocol::slot_remove {
  public:
    ~qw_slot_remove() override = default;

    constexpr qw_slot_remove( //
        size_t   index,       //
        uint64_t time,        //
        size_t   id_actor     //
        ) :
        slot_remove(index, time, id_actor) { }

    inline void perform(engine::access::world w) const override {
      verb(" :: event  Quadwar/slot_remove");

      root::slot_remove(w.get_root(), get_actor());

      w.remove(get_actor());
    }

    static inline auto decode(cref_vbyte seq) {
      return qw_slot_remove { get_index(seq), get_time(seq),
                              get_actor(seq) };
    }
  };
}

#endif
