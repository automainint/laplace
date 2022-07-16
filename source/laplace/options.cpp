#include "options.h"

#include <thread>

namespace laplace {
  using std::max, std::thread;

  auto default_thread_count() noexcept -> ptrdiff_t {
    static ptrdiff_t thread_count = max<ptrdiff_t>(
        0, thread::hardware_concurrency());
    return thread_count;
  }

  auto overthreading_limit() noexcept -> ptrdiff_t {
    return 8;
  }

  auto average_impact_list_size() noexcept -> ptrdiff_t {
    return 16;
  }

  auto average_async_impact_count() noexcept -> ptrdiff_t {
    return 1024;
  }

  auto average_sync_impact_count() noexcept -> ptrdiff_t {
    return 256;
  }

  auto average_fork_count() noexcept -> ptrdiff_t {
    return 128;
  }
}
