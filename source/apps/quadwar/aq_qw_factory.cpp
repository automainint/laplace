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

#include "protocol/qw_init.h"
#include "protocol/qw_launch.h"
#include "protocol/qw_player_name.h"
#include "protocol/qw_slot_create.h"
#include "protocol/qw_slot_remove.h"
#include "qw_factory.h"

namespace quadwar_app {
  using namespace protocol;

  auto qw_factory::decode(cref_vbyte seq) const
      -> engine::ptr_prime_impact {

    if (qw_slot_create::scan(seq))
      return make<qw_slot_create>(seq);
    if (qw_slot_remove::scan(seq))
      return make<qw_slot_remove>(seq);
    if (qw_player_name::scan(seq))
      return make<qw_player_name>(seq);
    if (qw_init::scan(seq))
      return make<qw_init>(seq);
    if (qw_launch::scan(seq))
      return make<qw_launch>(seq);

    return decode_native(seq);
  }
}
