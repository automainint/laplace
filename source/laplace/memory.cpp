/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "memory.h"

namespace laplace {
  using std::pmr::synchronized_pool_resource;

  synchronized_pool_resource memory_resource = {};
}
