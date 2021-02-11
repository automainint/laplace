/*  laplace/engine/object/eo_actor.cpp
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

#include "actor.h"
#include "sets.h"

namespace laplace::engine::object {
  size_t actor::n_is_local = 0;

  actor::actor(dummy_tag) : basic_entity(actor::dummy) { }

  actor::actor(proto_tag) {
    setup_sets({ { sets::is_local, 1, 0 } });

    n_is_local = index_of(sets::is_local);
  }

  void actor::set_local(access::entity en, bool is_local) {
    en.set(n_is_local, is_local);
  }

  auto actor::is_local(access::entity en) -> bool {
    return en.get(n_is_local) > 0 ? true : false;
  }
}
