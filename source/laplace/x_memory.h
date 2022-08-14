#ifndef LAPLACE_X_MEMORY_H
#define LAPLACE_X_MEMORY_H

#include <memory_resource>

namespace laplace {
  [[nodiscard]] auto default_memory_resource()
      -> std::pmr::memory_resource *;
}

#endif
