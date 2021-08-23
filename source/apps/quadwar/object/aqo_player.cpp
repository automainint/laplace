/*  apps/quadwar/object/aqo_player.cpp
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

#include "player.h"

#include "../../../laplace/core/serial.h"
#include "../../../laplace/core/utils.h"
#include "root.h"

namespace quadwar_app::object {
  using std::u8string_view, std::u8string;

  sl::index player::n_player_index = {};

  player player::m_proto(proto);

  player::player(proto_tag) : actor(proto) {
    setup_sets({ { .id = sets::player_index, .scale = 1 } });

    n_player_index = index_of(sets::player_index);
  }

  player::player(bool is_local) : actor(dummy) {
    *this = m_proto;

    init(n_is_local, is_local);
  }

  void player::init_indices(world w) {
    const auto r     = w.get_entity(w.get_root());
    const auto slots = w.get_entity(root::get_slots(r));

    const auto count = slots.vec_get_size();

    for (sl::index i = 0; i < count; i++) {
      const auto p = w.get_entity(as_index(slots.vec_get(i)));

      player::set_index(p, i);
      p.adjust();
    }
  }

  void player::set_name(entity en, u8string_view name) {
    en.bytes_resize(name.size());

    for (sl::index i = 0; i < name.size(); i++) {
      en.bytes_set(i, name[i]);
    }

    en.adjust();
  }

  void player::set_index(entity en, sl::index n) {
    en.set(n_player_index, n);
  }

  auto player::get_name(entity en) -> u8string {
    auto name = u8string {};

    for (sl::index i = 0, i1 = en.bytes_get_size(); i < i1; i++) {
      name.append(1, static_cast<char8_t>(en.bytes_get(i)));
    }

    return name;
  }

  auto player::get_index(entity en) -> sl::index {
    return en.get(n_player_index, -1);
  }
}
