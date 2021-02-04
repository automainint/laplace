/*  apps/quadwar/ids.h
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

#ifndef __quadwar__ids__
#define __quadwar__ids__

#include "../../laplace/engine/protocol/ids.h"

namespace quadwar_app::ids {
  using namespace laplace::engine::protocol::ids;

  enum command_id : uint16_t { player_name = _native_count };
}

#endif
