/*  laplace/engine/world.predef.h
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

#ifndef laplace_engine_world_predef_h
#define laplace_engine_world_predef_h

#include "../core/defs.h"
#include "basic_entity.predef.h"
#include "eval/random.h"
#include <functional>
#include <random>

namespace laplace::engine {
  using seed_type = eval::random::result_type;
  using ref_rand  = eval::random &;

  class world;
  using ref_world  = world &;
  using cref_world = const world &;
  using ptr_world  = std::shared_ptr<world>;
  using condition  = std::function<bool(cref_entity)>;
}

#endif
