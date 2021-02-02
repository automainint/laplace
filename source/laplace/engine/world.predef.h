#pragma once

#include "../core/defs.h"
#include "basic_entity.predef.h"
#include <functional>
#include <random>

namespace laplace::engine {
  using random_engine = std::default_random_engine;
  using seed_type     = random_engine::result_type;
  using ref_rand      = random_engine &;

  class world;
  using ref_world  = world &;
  using cref_world = const world &;
  using ptr_world  = std::shared_ptr<world>;
  using condition  = std::function<bool(cref_entity)>;
}
