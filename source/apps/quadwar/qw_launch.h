/*  apps/quadwar/qw_launch.h
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

#ifndef __quadwar__qw_launch__
#define __quadwar__qw_launch__

#include "../../laplace/engine/protocol/basic_event.h"
#include "root.h"

namespace quadwar_app {
  using namespace laplace;

  using qw_launch = engine::protocol::event_handler< //
      engine::protocol::server_launch,               //
      decltype([](engine::access::world w) {
        verb(" :: event  Quadwar/server_launch");

        root::launch(w.get_root());
      })>;
}

#endif