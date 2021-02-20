/*  apps/quadwar/game_clock.h
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

#ifndef __quadwar__game_clock__
#define __quadwar__game_clock__

#include "../../laplace/engine/basic_entity.h"

namespace quadwar_app {
  using namespace laplace;

  class game_clock : public engine::basic_entity {
  public:
    static constexpr size_t tick_period          = 100;
    static constexpr size_t server_tick_duration = 10;

    game_clock();
    ~game_clock() override = default;

    void tick(engine::access::world w) override;

  private:
    game_clock(proto_tag);

    static size_t n_clock_time;

    static game_clock m_proto;
  };
}

#endif
