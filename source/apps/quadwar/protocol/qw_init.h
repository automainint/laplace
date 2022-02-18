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

#include "../object/root.h"
#include "defs.h"

namespace quadwar_app::protocol {
  using init_lambda = decltype([](world w) {
    auto log = get_global_log();
    log(log_event::verbose, " :: event  Quadwar/server_init",
        "Quadwar");

    object::root::create(w);
  });

  using qw_init = event_handler<server_init, init_lambda>;
}

#endif
