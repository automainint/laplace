/*  laplace/network/defs.h
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

#ifndef laplace_network_defs_h
#define laplace_network_defs_h

#include "../core/defs.h"
#include "../core/socket.h"
#include <cstdint>

namespace laplace::network {
  static constexpr sl::whole max_chunk_size = 0x7fffffff;
  static constexpr auto      localhost      = "127.0.0.1";
  static constexpr uint16_t  any_port       = 0;

  enum io_mode { async, sync };
}

#endif
