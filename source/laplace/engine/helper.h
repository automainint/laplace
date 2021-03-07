/*  laplace/engine/helper.h
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

#ifndef laplace_engine_helper_h
#define laplace_engine_helper_h

#include "access/entity.h"
#include "access/ventity.h"
#include "access/world.h"

namespace laplace::engine {
  struct helper {
    using access_entity  = access::entity;
    using access_ventity = access::ventity;
    using access_world   = access::world;

    using entity  = const access::entity &;
    using ventity = const access::ventity &;
    using world   = const access::world &;
  };
}

#endif
