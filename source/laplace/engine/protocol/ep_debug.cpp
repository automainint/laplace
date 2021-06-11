/*  laplace/engine/protocol/ep_debug.cpp
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

#include "debug.h"

#include "../../core/utils.h"
#include "../object/sets.h"

namespace laplace::engine::protocol {
  using std::make_shared, object::sets::debug_value;

  void debug::perform(access::world w) const {
    auto root = w.get_entity(w.get_root());

    if (root.exist()) {
      root.set(root.index_of(debug_value), m_value);
    } else {
      auto e = make_shared<debug_root>();

      e->set(e->index_of(debug_value), m_value);

      w.set_root(w.spawn(e, engine::id_undefined));
    }
  }

  debug::debug_root::debug_root() {
    setup_sets({ { debug_value, 1, 0 } });
  }
}
