/*  apps/quadwar/protocol/qw_init.h
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

#ifndef quadwar_protocol_qw_init_h
#define quadwar_protocol_qw_init_h

#include "../object/game_clock.h"
#include "../object/landscape.h"
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  using qw_init = event_handler< //
      server_init,               //
      decltype([](world w) {
        verb(" :: event  Quadwar/server_init");

        auto root = std::make_shared<object::root>();

        w.set_root(w.spawn(root, engine::id_undefined));

        w.spawn(std::make_shared<object::game_clock>(),
                engine::id_undefined);

        auto land = std::make_shared<object::landscape>();

        object::root::set_landscape(
            { root, engine::access::sync },
            w.spawn(land, engine::id_undefined));

        object::landscape::generate(
            { land, engine::access::sync }, 20, 20);

        auto dist = std::uniform_int_distribution(0, 1);

        for (size_t i = 0; i < 20; i++) {
          for (size_t j = 0; j < 20; j++) {
            const auto tile = w.random(dist);
            object::landscape::set_tile(
                { land, engine::access::sync }, i, j, tile);
          }
        }
      })>;
}

#endif
