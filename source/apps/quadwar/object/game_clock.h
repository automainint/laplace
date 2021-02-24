/*  apps/quadwar/object/game_clock.h
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

#ifndef quadwar_object_game_clock_h
#define quadwar_object_game_clock_h

#include "../../../laplace/engine/basic_entity.h"
#include "defs.h"

namespace quadwar_app::object {
  class game_clock : public engine::basic_entity, helper {
  public:
    static constexpr size_t tick_period          = 100;
    static constexpr size_t server_tick_duration = 10;

    game_clock();
    ~game_clock() override = default;

    void tick(world w) override;

  private:
    game_clock(proto_tag);

    static size_t n_clock_time;

    static game_clock m_proto;
  };
}

#endif
