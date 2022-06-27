/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "options.h"

#include <thread>

namespace laplace {
  using std::max, std::thread;

  ptrdiff_t const default_thread_count = max<ptrdiff_t>(
      0, thread::hardware_concurrency() - 1);
  ptrdiff_t const overthreading_limit      = 8;
  ptrdiff_t const average_impact_list_size = 16;
  ptrdiff_t const average_impact_count     = 1024;
}
