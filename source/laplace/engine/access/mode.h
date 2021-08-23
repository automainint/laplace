/*  laplace/engine/access/mode.h
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

#ifndef laplace_engine_access_mode_h
#define laplace_engine_access_mode_h

#include "../../core/options.h"
#include <cstdint>

namespace laplace::engine::access {
  enum mode : uint8_t { forbidden, read_only, async, sync, data };

  [[nodiscard]] constexpr auto is_allowed(const mode operation,
                                          const mode status) noexcept
      -> bool {
    return operation <= status;
  }
}

#endif
