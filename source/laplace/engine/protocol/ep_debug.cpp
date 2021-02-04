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

#include "../../core/utils.h"
#include "../object/sets.h"
#include "debug.h"

namespace laplace::engine::protocol {
  using namespace object;
  using namespace std;

  void debug::perform(access::world w) const {
    verb(" :: event debug/Laplace");

    auto root = w.get_root();

    if (root.exist()) {
      root.set(root.index_of(sets::debug_value), m_value);
    } else {
      auto e = make_shared<debug_root>();

      e->set(e->index_of(sets::debug_value), m_value);

      w.set_root(e);
    }
  }

  debug::debug_root::debug_root() {
    setup_sets({ { sets::debug_value, 1, 0 } });
  }
}
