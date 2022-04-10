/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_HELPER_H
#define LAPLACE_ENGINE_HELPER_H

#include "access/entity.h"
#include "access/world.h"

namespace laplace::engine {
  struct helper {
    using access_entity = access::entity;
    using access_world  = access::world;

    using entity = const access::entity &;
    using world  = const access::world &;
  };
}

#endif
