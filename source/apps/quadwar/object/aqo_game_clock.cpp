/*  apps/quadwar/object/aqo_game_clock.cpp
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

#include "../../../laplace/engine/access/world.h"
#include "game_clock.h"
#include "sets.h"

namespace quadwar_app::object {
  using engine::basic_entity;

  size_t game_clock::n_clock_time = 0;

  game_clock game_clock::m_proto(basic_entity::proto);

  game_clock::game_clock(basic_entity::proto_tag) :
      basic_entity(true, false, false, false, tick_period) {

    setup_sets(                             //
        { { .id    = sets::game_clock_time, //
            .scale = sets::scale_time } });

    n_clock_time = index_of(sets::game_clock_time);
  }

  game_clock::game_clock() : basic_entity(dummy) {
    *this = m_proto;
  }

  void game_clock::tick(access_world) {
    const auto time    = get(n_clock_time) * server_tick_duration;
    const auto seconds = time / sets::scale_time;

    verb("Time: %jd:%02jd", seconds / 60, seconds % 60);

    apply_delta(n_clock_time, get_tick_period());
  }
}