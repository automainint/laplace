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

#include "../../../laplace/core/serial.h"
#include "player.h"

namespace quadwar_app::object {
  using std::u8string_view, std::u8string;

  player player::m_proto(player::proto);

  player::player(proto_tag) : object::actor(player::proto) { }

  player::player(bool is_local) : object::actor(player::dummy) {
    *this = m_proto;

    init(n_is_local, is_local);
  }

  void player::set_name(entity en, u8string_view name) {
    en.bytes_resize(name.size());

    for (sl::index i = 0; i < name.size(); i++) {
      en.bytes_set(i, name[i]);
    }

    en.adjust();
  }

  auto player::get_name(entity en) -> u8string {
    auto name = u8string {};

    for (sl::index i = 0, i1 = en.bytes_get_size(); i < i1; i++) {
      name.append(1, static_cast<char8_t>(en.bytes_get(i)));
    }

    return name;
  }
}
