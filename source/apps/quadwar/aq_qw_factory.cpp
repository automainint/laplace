/*  apps/quadwar/aq_factory.cpp
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

#include "qw_factory.h"
#include "qw_slot_create.h"
#include "qw_player_name.h"

namespace quadwar_app {
  auto qw_factory::decode(cref_vbyte seq) const
      -> engine::ptr_impact {
    if (qw_slot_create::scan(seq))
      return make<qw_slot_create>(seq);
    if (qw_player_name::scan(seq))
      return make<qw_player_name>(seq);

    return decode_native(seq);
  }
}
