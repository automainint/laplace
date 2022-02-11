/*  Copyright (c) 2022 Mitya Selivanov
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

namespace laplace::network {
  extern sl::whole const max_chunk_size;
  extern uint16_t const  any_port;

  enum io_mode { async, sync };
}

#endif
