/*  apps/quadwar/protocol/aqp_qw_loading.cpp
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

#include "qw_loading.h"

#include "../action/pathmap_reset.h"
#include "../action/unit_place.h"
#include "../object/game_clock.h"
#include "../object/landscape.h"
#include "../object/pathmap.h"
#include "../object/root.h"
#include "../object/unit.h"

namespace quadwar_app::protocol {
  using std::make_shared, object::root, object::pathmap, object::unit,
      object::game_clock, object::landscape, action::pathmap_reset,
      action::unit_place, engine::id_undefined, engine::ptr_impact;

  void qw_loading::perform(world w) const {
    verb(" :: event  Quadwar/loading");

    auto r = w.get_entity(w.get_root());

    root::loading(r);

    w.spawn(make_shared<game_clock>(), id_undefined);

    landscape::create_maze(w, m_map_size, m_map_size, m_player_count);

    auto id_path = pathmap::create(w);
    auto units   = unit::spawn_start_units(w, m_unit_count);

    auto ev          = ptr_impact {};
    auto child_count = size_t {};

    ev = make_shared<pathmap_reset>(id_path);
    ev->set_order(order_of_child(child_count));
    w.queue(ev);

    for (auto u : units) {
      ev = make_shared<unit_place>(u);
      ev->set_order(order_of_child(child_count));
      w.queue(ev);
    }
  }
}
