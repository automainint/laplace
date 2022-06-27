/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_MEMORY_H
#define LAPLACE_MEMORY_H

#include <memory_resource>

namespace laplace {
  extern std::pmr::synchronized_pool_resource memory_resource;
}

#endif
