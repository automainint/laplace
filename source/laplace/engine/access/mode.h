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

#ifndef __laplace__engine_access_mode__
#define __laplace__engine_access_mode__

#include <cstdint>

namespace laplace::engine::access {
  enum mode : uint8_t { forbidden, sync, async };
}

#endif
