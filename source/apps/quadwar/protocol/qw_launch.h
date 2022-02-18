/*  apps/quadwar/protocol/qw_launch.h
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

#ifndef quadwar_protocol_qw_launch_h
#define quadwar_protocol_qw_launch_h

#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  using qw_launch =
      event_handler<server_launch, decltype([](world w) {
                      auto log = get_global_log();
                      log(log_event::verbose,
                          " :: event  Quadwar/server_launch",
                          "Quadwar");

                      object::root::launch(
                          w.get_entity(w.get_root()));
                    })>;
}

#endif