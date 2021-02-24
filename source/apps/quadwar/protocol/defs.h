/*  apps/quadwar/protocol/defs.h
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

#ifndef quadwar_protocol_defs_h
#define quadwar_protocol_defs_h

#include "../../../laplace/engine/protocol/basic_event.h"
#include "../defs.h"
#include "ids.h"

namespace quadwar_app::protocol {
  using namespace laplace::engine::protocol;
  using world = laplace::engine::access::world;
}

#endif
