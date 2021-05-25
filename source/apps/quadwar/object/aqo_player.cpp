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
    en.modify(sets::player_name, serial::pack_to_bytes(name));
  }

  auto player::get_name(entity en) -> u8string {
    const auto data = en.request(sets::player_name);

    u8string result(data.size(), ' ');
    memcpy(result.data(), data.data(), data.size());

    return result;
  }

  auto player::do_request(sl::index id, span_cbyte args) const -> vbyte {
    if (id == sets::player_name && args.empty()) {
      return serial::pack_to_bytes(u8string_view { m_name });
    }

    return {};
  }

  void player::do_modify(sl::index id, span_cbyte args) {
    if (id == sets::player_name) {
      m_name.resize(args.size(), ' ');
      memcpy(m_name.data(), args.data(), args.size());
    }
  }
}
