#ifndef __laplace__engine_access_mode__
#define __laplace__engine_access_mode__

#include <cstdint>

namespace laplace::engine::access {
  enum mode : uint8_t { forbidden, sync, async };
}

#endif
