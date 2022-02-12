/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_DEFS_H
#define LAPLACE_NETWORK_DEFS_H

#include "../core/defs.h"

namespace laplace::network {
  static constexpr sl::index slot_host            = -1;
  static constexpr sl::index slot_count_unlimited = -1;
  static constexpr sl::index id_undefined         = -1;
  static constexpr sl::index index_undefined      = -1;
  static constexpr sl::time  time_undefined       = -1;

  extern sl::whole const max_chunk_size;
  extern uint16_t const  any_port;
}

#endif
