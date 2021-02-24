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
#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  using qw_init = event_handler< //
      server_init,               //
      decltype([](world w) {
        verb(" :: event  Quadwar/server_init");

        w.set_root(std::make_shared<object::root>());
        w.spawn(std::make_shared<object::game_clock>(),
                engine::id_undefined);
      })>;
}

#endif
