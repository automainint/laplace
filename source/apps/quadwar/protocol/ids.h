/*  apps/quadwar/protocol/ids.h
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

#ifndef quadwar_protocol_ids_h
#define quadwar_protocol_ids_h

#include "../../../laplace/engine/protocol/ids.h"

namespace quadwar_app::protocol::ids {
  using namespace laplace::engine::protocol::ids;

  enum command_id : uint16_t {
    player_name = _native_count,
    order_move
  };
}

#endif
