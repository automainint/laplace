/*  apps/quadwar/sets.h
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

#ifndef __quadwar__sets__
#define __quadwar__sets__

#include "../../laplace/engine/object/sets.h"

namespace quadwar_app::sets {
  using namespace laplace::engine::object::sets;

  enum set_id : size_t {
    player_name = _native_count,

    slot_count,
    slot_get,
    slot_create,
    slot_remove,
    status_changed
  };
}

#endif
