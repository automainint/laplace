/*  apps/quadwar/qw_init.h
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

#ifndef __quadwar__qw_init__
#define __quadwar__qw_init__

#include "../../laplace/engine/protocol/basic_event.h"
#include "root.h"
#include "game_clock.h"

namespace quadwar_app {
  using namespace laplace;

  using qw_init = engine::protocol::event_handler< //
      engine::protocol::server_init,               //
      decltype([](engine::access::world w) {
        verb(" :: event  Quadwar/server_init");

        w.set_root(std::make_shared<root>());
        w.spawn(std::make_shared<game_clock>(), engine::id_undefined);
      })>;
}

#endif
