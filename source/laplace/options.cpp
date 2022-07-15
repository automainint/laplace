#include "options.h"

#include <thread>

namespace laplace {
  using std::max, std::thread;

  ptrdiff_t const default_thread_count = max<ptrdiff_t>(
      0, thread::hardware_concurrency() - 1);
  ptrdiff_t const overthreading_limit        = 8;
  ptrdiff_t const average_impact_list_size   = 16;
  ptrdiff_t const average_async_impact_count = 1024;
  ptrdiff_t const average_sync_impact_count  = 256;
  ptrdiff_t const average_fork_count         = 128;
}
