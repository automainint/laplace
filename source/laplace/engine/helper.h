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
    using entity  = access::entity;
    using ventity = access::ventity;
    using world   = access::world;
  };
}

#endif
