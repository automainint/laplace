#include "x_memory.h"

namespace laplace {
  using std::pmr::memory_resource,
      std::pmr::synchronized_pool_resource;

  [[nodiscard]] auto default_memory_resource() -> memory_resource * {
    static synchronized_pool_resource resource;
    return &resource;
  }
}
